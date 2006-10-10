/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

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


TreeView::TreeView(QWidget *parent, const char *name)
        : KListView(parent, name),
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

//     resetColumns is called automatically
    for(uint i = 0; i != columns.size(); ++i)
    {
        addColumn(i18n(columns[i]));        
        setColumnWidthMode(i, QListView::Manual);
    }

    setColumnAlignment(col_status_ - 1, Qt::AlignCenter);
    if(KLSConfig::showMarkupStatus())
        setColumnAlignment(col_markup_ - 1, Qt::AlignCenter);
}

void TreeView::resetColumns()
{
    setColumnWidth(col_url_ - 1, (int)(0.45 * width()));
    
    setResizeMode(QListView::LastColumn); // fit to the window
    // resize again
    setColumnWidthMode(col_label_ - 1, QListView::Manual);
    setResizeMode(QListView::NoColumn);
}

double TreeView::columnsWidth() const
{
    kdDebug(23100) << "columns: " << columns() << endl;
    
    double width = 0.0;
    for(int i = 0; i != columns(); ++i)
    {
        kdDebug(23100) << "column width: " << columnWidth(i) << endl;
        width += columnWidth(i);
    }
    return width;
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

TreeViewItem::TreeViewItem(TreeView* parent, QListViewItem* after,
                           LinkStatus const* linkstatus)
        : KListViewItem(parent, after),
        last_child_(0), root_(parent)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(TreeView* root, QListViewItem* listview_item, QListViewItem* after,
                           LinkStatus const* linkstatus)
        : KListViewItem(listview_item, after),
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
        
        if(i + 1 == root_->urlColumnIndex()) {
            setText(item.columnIndex() - 1, KURL::decode_string(
                    KCharsets::resolveEntities(item.text(i + 1))));
        }
        else {
            setText(item.columnIndex() - 1, KCharsets::resolveEntities(item.text(i + 1)));
        }
        
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
        return linkStatus()->statusText();
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
    m_cg.setColor(QColorGroup::Text, color);

    KListViewItem::paintCell(p, m_cg, column, width, align);

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

QColor const& TreeColumnViewItem::textStatusColor() const
{
    if(columnIndex() == root_->urlColumnIndex() || columnIndex() == root_->statusColumnIndex())
    {
        if(linkStatus()->status() == LinkStatus::BROKEN)
            return Qt::red;
        else if(linkStatus()->status() == LinkStatus::HTTP_CLIENT_ERROR)
            return Qt::red;
        else if(linkStatus()->status() == LinkStatus::HTTP_REDIRECTION)
            return Qt::black;
        else if(linkStatus()->status() == LinkStatus::HTTP_SERVER_ERROR)
            return Qt::darkMagenta;
        else if(linkStatus()->status() == LinkStatus::MALFORMED)
            return Qt::red;
        else if(linkStatus()->status() == LinkStatus::NOT_SUPPORTED)
            return Qt::lightGray;
        else if(linkStatus()->status() == LinkStatus::SUCCESSFULL)
            return Qt::black;
        else if(linkStatus()->status() == LinkStatus::TIMEOUT)
            return Qt::darkMagenta;
        else if(linkStatus()->status() == LinkStatus::UNDETERMINED)
            return Qt::blue;
        
        return Qt::red;
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
                return linkStatus()->node()->content().simplifyWhiteSpace();
            else
                return linkStatus()->node()->url();
        }
        else
        {
            KURL url = linkStatus()->absoluteUrl();
            return Url::convertToLocal(linkStatus());
        }        
    }
    else if(column == root_->statusColumnIndex())
    {
        return QString();
    }
    else if(column == root_->labelColumnIndex())
    {
        QString label(linkStatus()->label());
        if(!label.isNull())
            return label.simplifyWhiteSpace();
    }
        
    return QString();
}

QPixmap TreeColumnViewItem::pixmap(int column) const
{
    Q_ASSERT(column > 0);

    if(column == root_->statusColumnIndex())
    {
        if(linkStatus()->status() == LinkStatus::BROKEN)
            return SmallIcon("no");
        else if(linkStatus()->status() == LinkStatus::HTTP_CLIENT_ERROR)
            return SmallIcon("no");
        else if(linkStatus()->status() == LinkStatus::HTTP_REDIRECTION) 
        {
            if(linkStatus()->statusText() == "304")
                return UserIcon("304");
            else
                return SmallIcon("redo");
        }
        else if(linkStatus()->status() == LinkStatus::HTTP_SERVER_ERROR)
            return SmallIcon("no");
        else if(linkStatus()->status() == LinkStatus::MALFORMED)
            return SmallIcon("editdelete");
        else if(linkStatus()->status() == LinkStatus::NOT_SUPPORTED)
            return SmallIcon("help");
        else if(linkStatus()->status() == LinkStatus::SUCCESSFULL)
            return SmallIcon("ok");
        else if(linkStatus()->status() == LinkStatus::TIMEOUT)
            return SmallIcon("history_clear");
        else if(linkStatus()->status() == LinkStatus::UNDETERMINED)
            return SmallIcon("help");
    }
    
    return QPixmap();
}


#include "treeview.moc"
