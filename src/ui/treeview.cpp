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
#include <QtDBus>
#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kurl.h>
#include <krun.h>
#include <kmessagebox.h>
#include <kcharsets.h>

#include <q3valuevector.h>
#include <q3header.h>
#include <QClipboard>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <Q3PopupMenu>

#include "treeview.h"
#include "global.h"
#include "engine/linkstatus.h"
#include "engine/linkfilter.h"
#include "klsconfig.h"


TreeView::TreeView(QWidget *parent, const char *name)
        : K3ListView(parent),
        ResultView(),
        current_column_(0)
{
    setShowToolTips(true);
    //setAllColumnsShowFocus(true);
    setSorting(1000); // don't start sorting any column
    setShowSortIndicator(true);
    //setFocusPolicy( WheelFocus );
    setRootIsDecorated(KLSConfig::displayTreeView());
//     setResizeMode(QListView::LastColumn);

    sub_menu_ = new Q3PopupMenu(this, "sub_menu_referrers");

    connect(this, SIGNAL( rightButtonClicked ( Q3ListViewItem *, const QPoint &, int )),
            this, SLOT( slotPopupContextMenu( Q3ListViewItem *, const QPoint &, int )) );
}


TreeView::~TreeView()
{
   saveLayout(KLSConfig::self()->config(), "klinkstatus");
}

void TreeView::setColumns(QStringList const& columns)
{
    ResultView::setColumns(columns);
    removeColunas();

//     resetColumns is called automatically
    for(uint i = 0; i != columns.size(); ++i)
    {
        addColumn(i18n(columns[i].toUtf8()));
        setColumnWidthMode(i, Q3ListView::Manual);
    }

    setColumnAlignment(col_status_ - 1, Qt::AlignCenter);
    if(KLSConfig::showMarkupStatus())
        setColumnAlignment(col_markup_ - 1, Qt::AlignCenter);
}

void TreeView::resetColumns()
{
    setColumnWidth(col_url_ - 1, (int)(0.45 * width()));

    setResizeMode(Q3ListView::LastColumn); // fit to the window
    // resize again
    setColumnWidthMode(col_label_ - 1, Q3ListView::Manual);
    setResizeMode(Q3ListView::NoColumn);
}

double TreeView::columnsWidth() const
{
    kDebug(23100) << "columns: " << columns() << endl;

    double width = 0.0;
    for(int i = 0; i != columns(); ++i)
    {
        kDebug(23100) << "column width: " << columnWidth(i) << endl;
        width += columnWidth(i);
    }
    return width;
}

void TreeView::clear()
{
    K3ListView::clear();
}

void TreeView::removeColunas()
{
    clear();
}

void TreeView::show(LinkStatusHelper::Status const& status)
{
    Q3ListViewItemIterator it(static_cast<K3ListView*> (this));
    while(it.current())
    {
        TreeViewItem* item = myItem(it.current());
	if(!item) return;
        if(!LinkStatusHelper(item->linkStatus()).hasStatus(status))
        {
            item->setVisible(false);
            //kDebug(23100) << "Hide: " << item->linkStatus()->absoluteUrl().url() << endl;
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
                    kDebug(23100) << "Show: " << parent->linkStatus()->absoluteUrl().url() << endl;

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
    Q3ListViewItemIterator it(this);
    while(it.current())
    {
        TreeViewItem* item = myItem(it.current());
	if(!item) return;
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
    Q3ListViewItemIterator it(this);
    while(it.current())
    {
        it.current()->setVisible(true);
        //it.current()->setEnabled(true);
        ++it;
    }
}

void TreeView::ensureRowVisible(const Q3ListViewItem * i, bool tree_display)
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
    K3ListView::resizeEvent(e);
    resetColumns();
    clipper()->repaint();
}

void TreeView::slotPopupContextMenu(Q3ListViewItem* item, const QPoint& pos, int col)
{
    current_column_ = col;

    TreeViewItem* tree_item = myItem(item);
    if(tree_item)
    {
        Q3ValueVector<KUrl> referrers = tree_item->linkStatus()->referrers();
        loadContextTableMenu(referrers, tree_item->linkStatus()->isRoot());
        context_table_menu_.popup(pos);
    }
}

void TreeView::slotCopyUrlToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;

    QString content(_item->linkStatus()->absoluteUrl().prettyUrl());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TreeView::slotCopyParentUrlToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    QString content(_item->linkStatus()->parent()->absoluteUrl().prettyUrl());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TreeView::slotCopyCellTextToClipboard() const
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    QString cell_text(_item->text(current_column_));
    QClipboard* cb = kapp->clipboard();
    cb->setText(cell_text);
}

void TreeView::slotEditReferrersWithQuanta()
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    Q3ValueVector<KUrl> referrers = _item->linkStatus()->referrers();

    if(Global::isQuantaAvailableViaDBUS())
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

    //kDebug(23100) << "id: " << id << endl;
    //kDebug(23100) << "index: " << index << endl;

    index -= 2; // The list of referrers starts on index = 2

    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    Q3ValueVector<KUrl> referrers = _item->linkStatus()->referrers();
    Q_ASSERT(index >= 0 && (uint)index < referrers.size());

    slotEditReferrerWithQuanta(referrers[index]);
}

void TreeView::slotEditReferrerWithQuanta(KUrl const& url)
{
    QString filePath = url.url();

    if(Global::isQuantaAvailableViaDBUS())
    {
#warning "kde4: port it"
#if 0
        DCOPRef quanta(Global::quantaDCOPAppId(),"WindowManagerIf");
        bool success = quanta.send("openFile", filePath, 0, 0);

        if(!success)
        {
            QString message = i18n("<qt>File <b>%1</b> cannot be opened. Might be a DCOP problem.</qt>", filePath);
            KMessageBox::error(parentWidget(), message);
        }
#endif
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
    if(!_item) return;
    KUrl url = _item->linkStatus()->absoluteUrl();

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
    if(!_item) return;
    if(_item->linkStatus()->isRoot())
    {
        KMessageBox::sorry(this, i18n("ROOT URL."));
    }
    else
    {
        LinkStatus const* ls_parent = _item->linkStatus()->parent();
        Q_ASSERT(ls_parent);

        KUrl url = ls_parent->absoluteUrl();

        if(url.isValid())
            (void) new KRun (url, 0, url.isLocalFile(), true);
        else
            KMessageBox::sorry(this, i18n("Invalid URL."));
    }
}

void TreeView::loadContextTableMenu(Q3ValueVector<KUrl> const& referrers, bool is_root)
{
    context_table_menu_.clear();
    sub_menu_->clear();

    if(!is_root)
    {
        sub_menu_->insertItem(i18n("All"), this, SLOT(slotEditReferrersWithQuanta()));
        sub_menu_->insertSeparator();

        for(uint i = 0; i != referrers.size(); ++i)
        {
            sub_menu_->insertItem(referrers[i].prettyUrl());
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

TreeViewItem* TreeView::myItem(Q3ListViewItem* item) const
{
    TreeViewItem* _item = dynamic_cast<TreeViewItem*> (item);
    return _item;
}


/* ******************************* TreeViewItem ******************************* */

TreeViewItem::TreeViewItem(TreeView* parent, Q3ListViewItem* after,
                           LinkStatus const* linkstatus)
        : K3ListViewItem(parent, after),
        last_child_(0), root_(parent)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(TreeView* root, Q3ListViewItem* listview_item, Q3ListViewItem* after,
                           LinkStatus const* linkstatus)
        : K3ListViewItem(listview_item, after),
        last_child_(0), root_(root)
{
    init(linkstatus);
}

TreeViewItem::~TreeViewItem()
{}

void TreeViewItem::init(LinkStatus const* linkstatus)
{
    setOpen(true);

    for(int i = 0; i != root_->numberOfColumns(); ++i)
    {
        TreeColumnViewItem item(root_, linkstatus, i + 1);
        column_items_.push_back(item);

        QString text(KCharsets::resolveEntities(item.text(i + 1)));

        if(i + 1 == root_->urlColumnIndex()) {
            setText(item.columnIndex() - 1, KUrl::decode_string(text));
        }
        else if(i + 1 == root_->statusColumnIndex()) {
            setText(item.columnIndex() - 1, i18n(text.toUtf8()));
        }
        else {
            setText(item.columnIndex() - 1, text);
        }

        setPixmap(item.columnIndex() - 1, item.pixmap(i + 1));
    }
}

void TreeViewItem::setLastChild(Q3ListViewItem* last_child)
{
    Q_ASSERT(last_child);
    last_child_ = last_child;
}

Q3ListViewItem* TreeViewItem::lastChild() const
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

LinkStatus const* TreeViewItem::linkStatus() const
{
    return column_items_[0].linkStatus();
}

void TreeViewItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
{
    TreeColumnViewItem item = column_items_[column];

    // Get a color to draw the text
    QColorGroup m_cg(cg);
    QColor color(item.textStatusColor());
    m_cg.setColor(QPalette::Text, color);

    K3ListViewItem::paintCell(p, m_cg, column, width, align);

    setHeight(22);
}


/* ******************************* TreeColumnViewItem ******************************* */

TreeColumnViewItem::TreeColumnViewItem(TreeView* root, LinkStatus const* linkstatus, int column_index)
    : root_(root), ls_(linkstatus), column_index_(column_index)
{
    Q_ASSERT(ls_);
//     Q_ASSERT(column_index_ > 0);
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

LinkStatus const* TreeColumnViewItem::linkStatus() const
{
    Q_ASSERT(ls_);
    return ls_;
}

QColor const TreeColumnViewItem::textStatusColor() const
{
    if(columnIndex() == root_->urlColumnIndex())
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

    else if(columnIndex() == root_->statusColumnIndex())
    {
        if(linkStatus()->errorOccurred())
        {
            //kDebug(23100) <<  "ERROR: " << linkStatus()->error() << ": " << linkStatus()->absoluteUrl().prettyUrl() << endl;
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
                kWarning(23100) <<  "status code == 0: " << endl;
                kWarning(23100) <<  LinkStatusHelper(linkStatus()).toString() << endl;
                kWarning(23100) <<  linkStatus()->httpHeader().toString() << endl;
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


    if(column == root_->urlColumnIndex())
    {
        if(linkStatus()->node() && linkStatus()->malformed())
        {
            if(linkStatus()->node()->url().isEmpty())
                return linkStatus()->node()->content().simplified();
            else
                return linkStatus()->node()->url();
        }
        else
        {
            KUrl url = linkStatus()->absoluteUrl();
            return Url::convertToLocal(linkStatus());
        }
    }
    else if(column == root_->statusColumnIndex())
    {
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
    }
    else if(column == root_->labelColumnIndex())
    {
        QString label(i18n((linkStatus()->label()).toUtf8()));
        if(!label.isNull())
            return label.simplified();
    }

    return QString();
}

QPixmap TreeColumnViewItem::pixmap(int column) const
{
    Q_ASSERT(column > 0);

    if(column == root_->statusColumnIndex())
    {
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
    }

    return QPixmap();
}


#include "treeview.moc"
