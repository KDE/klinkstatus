/***************************************************************************
 *   Copyright (C) 2004-2007 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "treeview.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kurl.h>
#include <krun.h>
#include <kmessagebox.h>
#include <kcharsets.h>
#include <kaction.h>

#include <QtDBus>
#include <QClipboard>
#include <QPixmap>
#include <QResizeEvent>
#include <QHeaderView>
#include <QScrollBar>

#include "engine/linkstatus.h"
#include "engine/linkfilter.h"
#ifdef HAVE_LIBTIDY
#include "tidy/markupvalidator.h"
#endif
#include "global.h"
#include "klsconfig.h"


TreeView::TreeView(QWidget *parent)
    : QTreeWidget(parent),
        ResultView(),
        current_column_(0)
{
//     setShowToolTips(true); // FIXME
//     setAllColumnsShowFocus(true);
    setSortingEnabled(false);
//     setShowSortIndicator(true);
//     setFocusPolicy( WheelFocus );
    setRootIsDecorated(KLSConfig::displayTreeView());
//     setAlternatingRowColors(true);
    // For status tip
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    // This one is *very* important for performance
    setUniformRowHeights(true);
//     setUpdatesEnabled(false);
    
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(slotCustomContextMenuRequested(const QPoint&)));
}


TreeView::~TreeView()
{
   // FIXME
//    saveLayout(KLSConfig::self()->config(), "klinkstatus");
}

void TreeView::setColumns(QStringList const& columns)
{
    ResultView::setColumns(columns);
    removeColumns();
    setColumnCount(columns.size());
    setHeaderLabels(columns);
    resetColumns();
}

void TreeView::resetColumns()
{
    setColumnWidth(col_url_ - 1, (int)(0.45 * width()));
    header()->setResizeMode(col_url_ - 1, QHeaderView::Interactive);

    header()->setResizeMode(col_status_ - 1, QHeaderView::ResizeToContents);

    if(KLSConfig::showMarkupStatus())
        header()->setResizeMode(col_markup_ - 1, QHeaderView::ResizeToContents);

    header()->setResizeMode(col_label_ - 1, QHeaderView::Stretch);

    // resize again
    header()->setResizeMode(col_label_ - 1, QHeaderView::Interactive);
}

double TreeView::columnsWidth() const
{
    kDebug(23100) << "number of columns: " << header()->count();

    double width = 0.0;
    for(int i = 0; i != header()->count(); ++i)
    {
        kDebug(23100) << "column width: " << columnWidth(i);
        width += columnWidth(i);
    }
    return width;
}

void TreeView::clear()
{
    QTreeWidget::clear();
}

void TreeView::removeColumns()
{
    clear();
}

void TreeView::show(LinkMatcher const& link_matcher)
{
    window()->setCursor(Qt::WaitCursor);
    
    setUpdatesEnabled(false);

    for(int i = 0; i != topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        setItemVisibleRecursively(item, link_matcher);
    }

    setUpdatesEnabled(true);

    window()->setCursor(Qt::ArrowCursor);
}

bool TreeView::isVisible(QTreeWidgetItem* item, LinkMatcher const& link_matcher) const
{
    if(link_matcher.matches(*(myItem(item)->linkStatus())))
        return true;

    for(int i = 0; i != item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        if(isVisible(child, link_matcher))
            return true;
    }

    return false;
}

void TreeView::setItemVisibleRecursively(QTreeWidgetItem* item, LinkMatcher const& link_matcher)
{
    bool visible = isVisible(item, link_matcher);
    item->setHidden(!visible);

    if(!visible)
        return;

    for(int i = 0; i != item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        setItemVisibleRecursively(child, link_matcher);
    }

    if(link_matcher.matches(*(myItem(item)->linkStatus()))) {
        item->setForeground(col_url_ - 1, QBrush(myItem(item)->foregroundColor(col_url_)));
        item->setForeground(col_label_ - 1, QBrush(myItem(item)->foregroundColor(col_label_)));
    }
    // doesn't match but has children who do
    else {
        item->setForeground(col_url_ - 1, QBrush(Qt::lightGray));
        item->setForeground(col_label_ - 1, QBrush(Qt::lightGray));
    }
}


void TreeView::showAll()
{
    for(int i = 0; i != topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        setItemVisibleRecursively(item, false);
        item->setHidden(false);
        item->setForeground(col_url_ - 1, QBrush(myItem(item)->foregroundColor(col_url_)));
        item->setForeground(col_label_ - 1, QBrush(myItem(item)->foregroundColor(col_label_)));
    }
}

QList<LinkStatus*> TreeView::getBrokenLinks() const
{
    LinkMatcher matcher(QString(), LinkStatusHelper::bad);
    return getLinksWithCriteria(matcher);
}

QList<LinkStatus*> TreeView::getVisibleLinks() const
{
    QList<LinkStatus*> items;
    for(int i = 0; i != topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        if(!item->isHidden()) {
            TreeViewItem* my_item = myItem(item);
            items.push_back(my_item->linkStatus());
        
            addVisibleItemsRecursively(items, my_item);
        }
    }
    return items;
}

void TreeView::addVisibleItemsRecursively(QList<LinkStatus*>& items, TreeViewItem* item) const
{
    for(int i = 0; i != item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        
        if(!child->isHidden()) {
            TreeViewItem* my_item = myItem(child);
            items.push_back(my_item->linkStatus());

            addVisibleItemsRecursively(items, my_item);
        }
    }
}

QList<LinkStatus*> TreeView::getLinksWithCriteria(LinkMatcher const& link_matcher) const
{
    QList<LinkStatus*> items;
    for(int i = 0; i != topLevelItemCount(); ++i) {
        TreeViewItem* my_item = myItem(topLevelItem(i));
        if(link_matcher.matches(*(my_item->linkStatus()))) {
            items.push_back(my_item->linkStatus());
        }
        addLinksWithCriteriaRecursively(items, my_item, link_matcher);
    }
    return items;
}

void TreeView::addLinksWithCriteriaRecursively(QList<LinkStatus*>& items, TreeViewItem* item, LinkMatcher const& link_matcher) const
{
    for(int i = 0; i != item->childCount(); ++i) {
        TreeViewItem* my_item = myItem(item->child(i));
        if(link_matcher.matches(*(my_item->linkStatus()))) {
            items.push_back(my_item->linkStatus());          
        }
        addLinksWithCriteriaRecursively(items, my_item, link_matcher);
    }
}

void TreeView::setItemVisibleRecursively(QTreeWidgetItem* item, bool hidden)
{
    item->setHidden(hidden);
    item->setForeground(col_url_ - 1, myItem(item)->foregroundColor(col_url_));
    item->setForeground(col_label_ - 1, myItem(item)->foregroundColor(col_label_));

    for(int i = 0; i != item->childCount(); ++i) {
        QTreeWidgetItem* child = item->child(i);
        setItemVisibleRecursively(child, hidden);
    }
}

void TreeView::ensureRowVisible(const QTreeWidgetItem* item, bool /*tree_display*/)
{
    scrollToItem(item, QAbstractItemView::PositionAtCenter);
}

bool TreeView::isEmpty() const
{
    return topLevelItemCount() == 0;
}

void TreeView::resizeEvent(QResizeEvent *e)
{
    QTreeWidget::resizeEvent(e);
    resetColumns();
}

void TreeView::slotPopupContextMenu(QTreeWidgetItem* item, QPoint const& point)
{
    TreeViewItem* tree_item = myItem(item);
    if(tree_item)
    {
        QSet<KUrl> const& referrers = tree_item->linkStatus()->referrers();
        loadContextTableMenu(referrers, tree_item->linkStatus()->isRoot());
        context_table_menu_.popup(viewport()->mapToGlobal(point));
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

void TreeView::slotEditReferrers()
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    QSet<KUrl> const& referrers = _item->linkStatus()->referrers();

    foreach(KUrl url, referrers) {
        (void) KRun::runUrl(url, QString("text/plain"), 0, false);
    }
}

void TreeView::slotEditReferrer(QAction* action)
{
    (void) KRun::runUrl(KUrl(action->text()), QString("text/plain"), 0, false);
}

void TreeView::slotViewUrlInBrowser()
{
    TreeViewItem* _item = myItem(currentItem());
    if(!_item) return;
    KUrl url = _item->linkStatus()->absoluteUrl();

    if(url.isValid())
    {
        (void) new KRun(url, 0, url.isLocalFile(), true);
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

void TreeView::loadContextTableMenu(QSet<KUrl> const& referrers, bool is_root)
{
    context_table_menu_.clear();

    context_table_menu_.addAction(KIcon("view-refresh"), i18n("Recheck"),
                                  this, SLOT(slotRecheckUrl()));
    context_table_menu_.addSeparator();
    
    delete(sub_menu_);

    sub_menu_ = context_table_menu_.addMenu(KIcon("document-properties"), i18n("Edit Referrer"));

    if(!is_root)
    {
        sub_menu_->addAction(i18n("All"), this, SLOT(slotEditReferrers()));
        sub_menu_->addSeparator();

        foreach(KUrl url, referrers)
        {
            sub_menu_->addAction(url.prettyUrl());
        }
        connect(sub_menu_, SIGNAL(triggered(QAction*)), this, SLOT(slotEditReferrer(QAction*)));
    }
    else
    {
        QMenu* sub_menu = context_table_menu_.addMenu(KIcon("document-properties"), i18n("Edit Referrer"));
        sub_menu->setEnabled(false);
    }
    context_table_menu_.addSeparator();

    context_table_menu_.addAction(KIcon("document-open"), i18n("Open URL"),
                                  this, SLOT(slotViewUrlInBrowser()));
    context_table_menu_.addAction(/*KIcon("document-open"), */i18n("Open Referrer URL"),
                                  this, SLOT(slotViewParentUrlInBrowser()));

    context_table_menu_.addSeparator();

    context_table_menu_.addAction(KIcon("edit-copy"), i18n("Copy URL"),
                                  this, SLOT(slotCopyUrlToClipboard()));
    context_table_menu_.addAction(/*KIcon("edit-copy"), */i18n("Copy Referrer URL"),
                                  this, SLOT(slotCopyParentUrlToClipboard()));
    context_table_menu_.addAction(/*KIcon("edit-copy"), */i18n("Copy Cell Text"),
                                  this, SLOT(slotCopyCellTextToClipboard()));
}

void TreeView::slotRecheckUrl()
{
    TreeViewItem* item = myItem(currentItem());
    if(!item)
        return;
    
    emit signalLinkRecheck(item->linkStatus());
}

TreeViewItem* TreeView::myItem(QTreeWidgetItem* item)
{
    return static_cast<TreeViewItem*> (item);
}

void TreeView::slotItemClicked(QTreeWidgetItem*, int column)
{
    current_column_ = column;
}

void TreeView::slotCustomContextMenuRequested(const QPoint& point)
{
    QTreeWidgetItem* item = itemAt(point);
    slotPopupContextMenu(item, point);
}

/* ******************************* TreeViewItem ******************************* */

TreeViewItem::TreeViewItem(TreeView* parent, LinkStatus* linkstatus)
        : QTreeWidgetItem(parent),
        last_child_(0), root_(parent)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(TreeView* root, QTreeWidgetItem* listview_item,
                           LinkStatus* linkstatus)
        : QTreeWidgetItem(listview_item),
        last_child_(0), root_(root)
{
    init(linkstatus);
}

TreeViewItem::~TreeViewItem()
{}

void TreeViewItem::init(LinkStatus* linkstatus)
{
    setExpanded(true);

    for(int i = 0; i != root_->numberOfColumns(); ++i)
    {
        TreeColumnViewItem item(root_, linkstatus, i + 1);
        column_items_.push_back(item);

        QString text(KCharsets::resolveEntities(item.text(i + 1)));

        if(i + 1 == root_->urlColumnIndex()) {
            setText(item.columnIndex() - 1, KUrl::fromPercentEncoding(text.toUtf8()));
            setStatusTip(i, QTreeWidgetItem::text(i));
        }
        else if(i + 1 == root_->statusColumnIndex()) {
            setText(item.columnIndex() - 1, text.toUtf8());
            setStatusTip(i, linkstatus->statusText());
            setToolTip(i, linkstatus->statusText());
        }
        else {
            setText(item.columnIndex() - 1, text);
            setStatusTip(i, QTreeWidgetItem::text(i));
        }

        setIcon(item.columnIndex() - 1, item.pixmap(i + 1));
        setForeground(item.columnIndex() - 1, QBrush(foregroundColor(item.columnIndex())));
        setBackground(item.columnIndex() - 1, QBrush(backgroundColor(item.columnIndex())));
    }
}

void TreeViewItem::refresh(LinkStatus* linkstatus)
{
    column_items_.clear();
    init(linkstatus);
}

void TreeViewItem::setLastChild(QTreeWidgetItem* last_child)
{
    Q_ASSERT(last_child);
    last_child_ = last_child;
}

QTreeWidgetItem* TreeViewItem::lastChild() const
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

LinkStatus* TreeViewItem::linkStatus() const
{
    return column_items_[0].linkStatus();
}

QColor const TreeViewItem::foregroundColor(int columnIndex) const
{
    if(columnIndex == root_->urlColumnIndex() || columnIndex == root_->statusColumnIndex())
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

QColor const TreeViewItem::backgroundColor(int columnIndex) const
{
    if(columnIndex != root_->urlColumnIndex()) {
      return Qt::transparent;
    }

    if(!linkStatus()->local()) {
        QColor color(Qt::blue);
        color.setAlpha(50);
        return color.lighter();
    }
    else {
        return Qt::transparent;
    }
}


/* ******************************* TreeColumnViewItem ******************************* */

TreeColumnViewItem::TreeColumnViewItem(TreeView* root, LinkStatus* linkstatus, int column_index)
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

LinkStatus* TreeColumnViewItem::linkStatus() const
{
    Q_ASSERT(ls_);
    return ls_;
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
        return QString::null;
    }
    else if(column == root_->labelColumnIndex())
    {
        QString label(linkStatus()->label());
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
        if(linkStatus()->status() == LinkStatus::BROKEN)
            return SmallIcon("dialog-error");
        else if(linkStatus()->status() == LinkStatus::HTTP_CLIENT_ERROR)
            return SmallIcon("dialog-error");
        else if(linkStatus()->status() == LinkStatus::HTTP_REDIRECTION)
        {
            if(linkStatus()->statusText() == "304")
                return SmallIcon("dialog-ok");
            else
                return SmallIcon("edit-redo");
        }
        else if(linkStatus()->status() == LinkStatus::HTTP_SERVER_ERROR)
            return SmallIcon("dialog-error");
        else if(linkStatus()->status() == LinkStatus::MALFORMED)
            return SmallIcon("dialog-warning");
        else if(linkStatus()->status() == LinkStatus::NOT_SUPPORTED)
            return SmallIcon("unknown");
        else if(linkStatus()->status() == LinkStatus::SUCCESSFULL)
            return SmallIcon("dialog-ok");
        else if(linkStatus()->status() == LinkStatus::TIMEOUT)
            return SmallIcon("chronometer");
        else if(linkStatus()->status() == LinkStatus::UNDETERMINED)
            return SmallIcon("dialog-error");
    }
    else if(column == root_->markupColumnIndex())
    {
        if(linkStatus()->hasHtmlErrors())
            return SmallIcon("dialog-error");
        else if(linkStatus()->hasHtmlWarnings())
            return SmallIcon("dialog-warning");
    }

    return QPixmap();
}


#include "treeview.moc"
