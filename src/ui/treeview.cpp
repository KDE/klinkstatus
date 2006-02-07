//
// C++ Implementation: treeview
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kurl.h>
#include <krun.h>
#include <dcopref.h>
#include <kmessagebox.h>
#include <dcopclient.h>
#include <kcharsets.h>

#include <qvaluevector.h>
#include <qheader.h>
#include <qclipboard.h>

#include "treeview.h"
#include "../global.h"
#include "../engine/linkstatus.h"
#include "../engine/linkfilter.h"
#include "../cfg/klsconfig.h"


TreeView::TreeView(QWidget *parent, const char *name,
                   int column_index_status,
                   int column_index_label,
                   int column_index_URL)
        : KListView(parent, name),
        ResultView(column_index_status, column_index_label, column_index_URL),
        current_column_(0)
{
    setShowToolTips(true);
    //setAllColumnsShowFocus(true);
    setSorting(1000); // don't start sorting any column
    setShowSortIndicator(true);
    //setFocusPolicy( WheelFocus );
    setRootIsDecorated(KLSConfig::displayTreeView());
    //setResizeMode(QListView::LastColumn);

    sub_menu_ = new QPopupMenu(this, "sub_menu_referrers");
    
    connect(this, SIGNAL( rightButtonClicked ( QListViewItem *, const QPoint &, int )),
            this, SLOT( slotPopupContextMenu( QListViewItem *, const QPoint &, int )) );
}


TreeView::~TreeView()
{
    saveLayout(KLSConfig::self()->config(), "klinkstatus");
}

void TreeView::setColumns(QStringList const& columns)
{
    ResultView::setColumns(columns);
    removeColunas();

    // resetColumns is called automatically
    for(uint i = 0; i != columns.size(); ++i)
    {
        if(i == 0)
        {
            Q_ASSERT(columns[i] == i18n("URL") && col_url_ == 1);
            addColumn(i18n(columns[i]));
        }
        else if(i == 1)
        {
            Q_ASSERT(columns[i] == i18n("Status") && col_status_ == 2);
            addColumn(i18n(columns[i]), 48);
        }
        else if(i == 2)
        {
            Q_ASSERT(columns[i] == i18n("Label") && col_label_ == 3);
            addColumn(i18n(columns[i])/*, (int)(0.45 * width() - 79)*/);
        }

        setColumnWidthMode(i, QListView::Manual);
    }

    setColumnAlignment(col_status_ - 1, Qt::AlignCenter);
}

void TreeView::resetColumns()
{
    setColumnWidth(col_url_ - 1, (int)(0.55 * width()));
    setColumnWidth(col_label_ - 1, (int)(0.45 * width()/* - 79*/));
}

void TreeView::clear()
{
    KListView::clear();
}

void TreeView::removeColunas()
{
    clear();
}

void TreeView::show(ResultView::Status const& status)
{
    QListViewItemIterator it(static_cast<KListView*> (this));
    while(it.current())
    {
        TreeViewItem* item = myItem(it.current());
        if(!ResultView::displayableWithStatus(item->linkStatus(), status))
        {
            item->setVisible(false);
            //kdDebug(23100) << "Hide: " << item->linkStatus()->absoluteUrl().url() << endl;
        }
        else
        {
            item->setVisible(true);
            //item->setEnabled(true);
            /*
            if(KLSConfig::displayTreeView() && status != ResultView::good && item->parent())
            {
                TreeViewItem* parent = myItem(item->parent());
                while(parent)
                {
                    kdDebug(23100) << "Show: " << parent->linkStatus()->absoluteUrl().url() << endl;

                    parent->setVisible(true);
                    //parent->setEnabled(false);
                    
                    if(parent->parent())
                        parent = myItem(parent->parent());
                    else
                        parent = 0;
                }
            }
            */
        }
//         
        ++it;
    }
}

void TreeView::show(LinkMatcher link_matcher)
{
    QListViewItemIterator it(this);
    while(it.current())
    {
        TreeViewItem* item = myItem(it.current());
        bool match = link_matcher.matches(*(item->linkStatus()));
        
        if(tree_display_)
            item->setEnabled(match);
        else
            item->setVisible(match);
        
        ++it;
    }
}

void TreeView::showAll()
{
    QListViewItemIterator it(this);
    while(it.current())
    {
        it.current()->setVisible(true);
        //it.current()->setEnabled(true);
        ++it;
    }
}

void TreeView::ensureRowVisible(const QListViewItem * i, bool tree_display)
{
    QScrollBar* vertical_scroll_bar = verticalScrollBar();

    if(tree_display ||
            vertical_scroll_bar->value() > (vertical_scroll_bar->maxValue() - vertical_scroll_bar->lineStep()))
        ensureItemVisible(i);
}

bool TreeView::isEmpty() const
{
    return !childCount();
}

void TreeView::resizeEvent(QResizeEvent *e)
{
    KListView::resizeEvent(e);
    resetColumns();
    clipper()->repaint();
}

void TreeView::slotPopupContextMenu(QListViewItem* item, const QPoint& pos, int col)
{
    current_column_ = col;
    
    TreeViewItem* tree_item = myItem(item);
    if(tree_item)
    {
        QValueVector<KURL> referrers = tree_item->linkStatus()->referrers();
        loadContextTableMenu(referrers, tree_item->linkStatus()->isRoot());
        context_table_menu_.popup(pos);
    }
}

void TreeView::slotCopyUrlToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    QString content(_item->linkStatus()->absoluteUrl().prettyURL());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TreeView::slotCopyParentUrlToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    QString content(_item->linkStatus()->parent()->absoluteUrl().prettyURL());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TreeView::slotCopyCellTextToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    QString cell_text(_item->text(current_column_));
    QClipboard* cb = kapp->clipboard();
    cb->setText(cell_text);
}

void TreeView::slotEditReferrersWithQuanta()
{
    TreeViewItem* _item = myItem(currentItem());
    QValueVector<KURL> referrers = _item->linkStatus()->referrers();

    if(Global::isQuantaAvailableViaDCOP())
    {
        for(uint i = 0; i != referrers.size(); ++i)
            slotEditReferrerWithQuanta(referrers[i]);
    }
    else
    {
        QStringList list_urls;

        for(uint i = 0; i != referrers.size(); ++i)
            list_urls.append(referrers[i].url());

        Global::openQuanta(list_urls);
    }
}

void TreeView::slotEditReferrerWithQuanta(int id)
{
    int index = sub_menu_->indexOf(id);

    if(index == 0)
        return;
    Q_ASSERT(index != -1);
    Q_ASSERT(index != 1); // separator

    //kdDebug(23100) << "id: " << id << endl;
    //kdDebug(23100) << "index: " << index << endl;

    index -= 2; // The list of referrers starts on index = 2

    TreeViewItem* _item = myItem(currentItem());
    QValueVector<KURL> referrers = _item->linkStatus()->referrers();
    Q_ASSERT(index >= 0 && (uint)index < referrers.size());

    slotEditReferrerWithQuanta(referrers[index]);
}

void TreeView::slotEditReferrerWithQuanta(KURL const& url)
{
    QString filePath = url.url();

    if(Global::isQuantaAvailableViaDCOP())
    {
        DCOPRef quanta(Global::quantaDCOPAppId(),"WindowManagerIf");
        bool success = quanta.send("openFile", filePath, 0, 0);

        if(!success)
        {
            QString message = i18n("<qt>File <b>%1</b> cannot be opened. Might be a DCOP problem.</qt>").arg(filePath);
            KMessageBox::error(parentWidget(), message);
        }
    }
    else
    {
        QStringList args(url.url());
        Global::openQuanta(args);
    }
}

void TreeView::slotViewUrlInBrowser()
{
    TreeViewItem* _item = myItem(currentItem());
    KURL url = _item->linkStatus()->absoluteUrl();

    if(url.isValid())
    {
        (void) new KRun (url, 0, url.isLocalFile(), true);
    }
    else
        KMessageBox::sorry(this, i18n("Invalid URL."));
}

void TreeView::slotViewParentUrlInBrowser()
{
    TreeViewItem* _item = myItem(currentItem());
 
    if(_item->linkStatus()->isRoot())
    {
        KMessageBox::sorry(this, i18n("ROOT URL."));
    }
    else
    {
        LinkStatus const* ls_parent = _item->linkStatus()->parent();
        Q_ASSERT(ls_parent);

        KURL url = ls_parent->absoluteUrl();

        if(url.isValid())
            (void) new KRun (url, 0, url.isLocalFile(), true);
        else
            KMessageBox::sorry(this, i18n("Invalid URL."));
    }
}

void TreeView::loadContextTableMenu(QValueVector<KURL> const& referrers, bool is_root)
{
    context_table_menu_.clear();
    sub_menu_->clear();

    if(!is_root)
    {
        sub_menu_->insertItem(i18n("All"), this, SLOT(slotEditReferrersWithQuanta()));
        sub_menu_->insertSeparator();

        for(uint i = 0; i != referrers.size(); ++i)
        {
            sub_menu_->insertItem(referrers[i].prettyURL());
        }
        connect(sub_menu_, SIGNAL(activated(int)), this, SLOT(slotEditReferrerWithQuanta(int)));

        context_table_menu_.insertItem(SmallIconSet("edit"), i18n("Edit Referrer with Quanta"),
                                       sub_menu_);
        context_table_menu_.insertSeparator();
    }
    else
    {
        int id = context_table_menu_.insertItem(SmallIconSet("fileopen"), i18n("Edit Referrer with Quanta"));
        context_table_menu_.setItemEnabled(id, false);
    }

    context_table_menu_.insertItem(SmallIconSet("fileopen"), i18n("Open URL"),
                                   this, SLOT(slotViewUrlInBrowser()));

    context_table_menu_.insertItem(/*SmallIconSet("fileopen"), */i18n("Open Referrer URL"),
                                   this, SLOT(slotViewParentUrlInBrowser()));

    context_table_menu_.insertSeparator();

    context_table_menu_.insertItem(SmallIconSet("editcopy"), i18n("Copy URL"),
                                   this, SLOT(slotCopyUrlToClipboard()));

    context_table_menu_.insertItem(/*SmallIconSet("editcopy"), */i18n("Copy Referrer URL"),
                                   this, SLOT(slotCopyParentUrlToClipboard()));

    context_table_menu_.insertItem(/*SmallIconSet("editcopy"), */i18n("Copy Cell Text"),
                                   this, SLOT(slotCopyCellTextToClipboard()));
}

TreeViewItem* TreeView::myItem(QListViewItem* item) const
{
    TreeViewItem* _item = dynamic_cast<TreeViewItem*> (item);
    Q_ASSERT(_item);
    return _item;
}


/* ******************************* TreeViewItem ******************************* */

TreeViewItem::TreeViewItem(QListView* listview, QListViewItem* after,
                           LinkStatus const* linkstatus, int number_of_columns)
        : KListViewItem(listview, after), number_of_columns_(number_of_columns),
        last_child_(0)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(QListViewItem* listview_item, QListViewItem* after,
                           LinkStatus const* linkstatus, int number_of_columns)
        : KListViewItem(listview_item, after), number_of_columns_(number_of_columns),
        last_child_(0)

{
    init(linkstatus);
}

TreeViewItem::~TreeViewItem()
{}

void TreeViewItem::init(LinkStatus const* linkstatus)
{
    setOpen(true);

    for(int i = 0; i != number_of_columns_; ++i)
    {
        TreeColumnViewItem item(linkstatus, i + 1);
        column_items_.push_back(item);
        setText(item.columnIndex() - 1, KCharsets::resolveEntities(item.text(i + 1)));
        setPixmap(item.columnIndex() - 1, item.pixmap(i + 1));
    }
}

void TreeViewItem::setLastChild(QListViewItem* last_child)
{
    Q_ASSERT(last_child);
    last_child_ = last_child;
}

QListViewItem* TreeViewItem::lastChild() const
{
    return last_child_;
}

QString TreeViewItem::key(int column, bool) const
{
    // FIXME magic numbers
    switch(column)
    {
    case 1: // status column
        return linkStatus()->status();
    }

    return text(column);
}

LinkStatus const* const TreeViewItem::linkStatus() const
{
    return column_items_[0].linkStatus();
}

void TreeViewItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
{
    TreeColumnViewItem item = column_items_[column];

    // Get a color to draw the text
    QColorGroup m_cg(cg);
    QColor color(item.textStatusColor());
    m_cg.setColor(QColorGroup::Text, color);

    KListViewItem::paintCell(p, m_cg, column, width, align);

    setHeight(22);
}


/* ******************************* TreeColumnViewItem ******************************* */

TreeColumnViewItem::TreeColumnViewItem(LinkStatus const* linkstatus, int column_index)
        : ls_(linkstatus), column_index_(column_index)
{
    Q_ASSERT(ls_);
    Q_ASSERT(column_index_ > 0);
}

TreeColumnViewItem::~TreeColumnViewItem()
{}

/*
void TreeColumnViewItem::setColumnIndex(int i)
{
    Q_ASSERT(i > 0);
    column_index_ = i;
}
*/
int TreeColumnViewItem::columnIndex() const
{
    return column_index_;
}

LinkStatus const* const TreeColumnViewItem::linkStatus() const
{
    Q_ASSERT(ls_);
    return ls_;
}

QColor const& TreeColumnViewItem::textStatusColor() const
{
    if(columnIndex() == 1) // URL col
    {
        QString status_code(QString::number(linkStatus()->httpHeader().statusCode()));

        if(linkStatus()->errorOccurred())
        {
            if(linkStatus()->error().contains("Timeout"))
                return Qt::darkMagenta;
            else if(linkStatus()->error().contains("not suported"))
                return Qt::lightGray;
            else
                return Qt::red;
        }
        else if(linkStatus()->absoluteUrl().protocol() != "http" &&
                linkStatus()->absoluteUrl().protocol() != "https")
            return Qt::black;

        else if(status_code[0] == '5')
            return Qt::darkMagenta;

        else if(status_code[0] == '4')
            return Qt::red;

        else
            return Qt::black;
    }

    else if(columnIndex() == 2) // Status col
    {
        if(linkStatus()->errorOccurred())
        {
            //kdDebug(23100) <<  "ERROR: " << linkStatus()->error() << ": " << linkStatus()->absoluteUrl().prettyURL() << endl;
            if(linkStatus()->error() == "Javascript not suported")
                return Qt::lightGray;
            else
                return Qt::red;
        }

        else if(linkStatus()->absoluteUrl().hasRef())
            return Qt::blue;

        else if(linkStatus()->absoluteUrl().protocol() != "http" &&
                linkStatus()->absoluteUrl().protocol() != "https")
            return Qt::darkGreen;

        else
        {
            QString status_code(QString::number(linkStatus()->httpHeader().statusCode()));

            if(status_code[0] == '0')
            {
                kdWarning(23100) <<  "status code == 0: " << endl;
                kdWarning(23100) <<  linkStatus()->toString() << endl;
                kdWarning(23100) <<  linkStatus()->httpHeader().toString() << endl;
            }
            //Q_ASSERT(status_code[0] != '0');

            if(status_code[0] == '5')
                return Qt::darkMagenta;

            else if(status_code[0] == '4')
                return Qt::red;

            else if(status_code[0] == '3')
                return Qt::blue;

            else if(status_code[0] == '2')
                return Qt::darkGreen;

            else
                return Qt::red;
        }
    }
    else
        return Qt::black;
}

QString TreeColumnViewItem::text(int column) const
{
    Q_ASSERT(column > 0);

    switch(column)
    {
    case 1: // URL column
        if(linkStatus()->node() && linkStatus()->malformed())
        {
            if(linkStatus()->node()->url().isEmpty())
                return linkStatus()->node()->content().simplifyWhiteSpace();
            else
                return linkStatus()->node()->url();
        }
        else
        {
            KURL url = linkStatus()->absoluteUrl();
            return Url::convertToLocal(linkStatus());
        }
        break;

    case 2: // Status column
        if(linkStatus()->errorOccurred() ||
                linkStatus()->status() == "OK" ||
                linkStatus()->status() == "304")
        {
            return QString();
        }
        else
        {
            return linkStatus()->status();
        }
        break;

    case 3: // Label column
        QString label(linkStatus()->label());
        if(!label.isNull())
            return label.simplifyWhiteSpace();
        break;
        /*
        default:
        kdError() << "TreeColumnViewItem::text: Wrong Column Number - " << column << endl;
        retureturn QString()rn QString();
        */
    }
    return QString();
}

QPixmap TreeColumnViewItem::pixmap(int column) const
{
    Q_ASSERT(column > 0);

    switch(column)
    {
    case 1: // URL column
        return QPixmap();
        break;

    case 2: // Status column
        if(linkStatus()->errorOccurred())
        {

            if(linkStatus()->error().contains("Timeout"))
            {
                return SmallIcon("kalarm");
            }
            else if(linkStatus()->error() == "Malformed")
            {
                return SmallIcon("bug");
            }
            else
            {
                return SmallIcon("no");
            }
        }
        else if(linkStatus()->status() == "304")
            return UserIcon("304");

        else if(linkStatus()->status() == "OK")
            return SmallIcon("ok");
        break;

    case 3: // Label column
        return QPixmap();
        break;

    default:
        kdError() << "TreeColumnViewItem::pixmap: Wrong Column Number - " << column << endl;
        return QPixmap();
    }
    return QPixmap();
}


#include "treeview.moc"
