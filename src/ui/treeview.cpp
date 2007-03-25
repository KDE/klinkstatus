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
#include <kmessagebox.h>
#include <kcharsets.h>
#include <kaction.h>

#include <QtDBus>
#include <q3valuelist.h>
#include <q3header.h>
#include <QClipboard>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <Q3PopupMenu>
#include <QHeaderView>
#include <QScrollBar>

#include "treeview.h"
#include "global.h"
#include "engine/linkstatus.h"
#include "engine/linkfilter.h"
#include "klsconfig.h"
#include "tidy/markupvalidator.h"


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
    setAlternatingRowColors(true);
    setMouseTracking(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    
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
    removeColunas();
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
    kDebug(23100) << "number of columns: " << header()->count() << endl;

    double width = 0.0;
    for(int i = 0; i != header()->count(); ++i)
    {
        kDebug(23100) << "column width: " << columnWidth(i) << endl;
        width += columnWidth(i);
    }
    return width;
}

void TreeView::clear()
{
    QTreeWidget::clear();
}

void TreeView::removeColunas()
{
    clear();
}

void TreeView::show(LinkMatcher const& link_matcher)
{
    for(int i = 0; i != topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        setItemVisibleRecursively(item, link_matcher);
    }
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
}


void TreeView::showAll()
{
    for(int i = 0; i != topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        setItemVisibleRecursively(item, false);
        item->setHidden(false);
    }
}

void TreeView::setItemVisibleRecursively(QTreeWidgetItem* item, bool hidden)
{
    setHidden(hidden);

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
        Q3ValueList<KUrl> referrers = tree_item->linkStatus()->referrers();
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
    Q3ValueList<KUrl> referrers = _item->linkStatus()->referrers();

    for(int i = 0; i != referrers.size(); ++i) {
        (void) KRun::runUrl(referrers[i], QString("text/plain"), 0, false);
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

void TreeView::loadContextTableMenu(Q3ValueList<KUrl> const& referrers, bool is_root)
{
    context_table_menu_.clear();
    delete(sub_menu_);

    sub_menu_ = context_table_menu_.addMenu(SmallIconSet("edit"), i18n("Edit Referrer"));

    if(!is_root)
    {
        sub_menu_->addAction(i18n("All"), this, SLOT(slotEditReferrers()));
        sub_menu_->addSeparator();

        for(int i = 0; i != referrers.size(); ++i)
        {
            sub_menu_->addAction(referrers[i].prettyUrl());
        }
        connect(sub_menu_, SIGNAL(triggered(QAction*)), this, SLOT(slotEditReferrer(QAction*)));
    }
    else
    {
        QMenu* sub_menu = context_table_menu_.addMenu(SmallIconSet("document-open"), i18n("Edit Referrer"));
        sub_menu->setEnabled(false);
    }
    context_table_menu_.addSeparator();

    context_table_menu_.addAction(SmallIconSet("document-open"), i18n("Open URL"),
                                  this, SLOT(slotViewUrlInBrowser()));
    context_table_menu_.addAction(/*SmallIconSet("document-open"), */i18n("Open Referrer URL"),
                                  this, SLOT(slotViewParentUrlInBrowser()));

    context_table_menu_.addSeparator();

    context_table_menu_.addAction(SmallIconSet("edit-copy"), i18n("Copy URL"),
                                  this, SLOT(slotCopyUrlToClipboard()));
    context_table_menu_.addAction(/*SmallIconSet("edit-copy"), */i18n("Copy Referrer URL"),
                                  this, SLOT(slotCopyParentUrlToClipboard()));
    context_table_menu_.addAction(/*SmallIconSet("edit-copy"), */i18n("Copy Cell Text"),
                                  this, SLOT(slotCopyCellTextToClipboard()));
}

TreeViewItem* TreeView::myItem(QTreeWidgetItem* item) const
{
    TreeViewItem* _item = static_cast<TreeViewItem*> (item);
    return _item;
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

TreeViewItem::TreeViewItem(TreeView* parent, LinkStatus const* linkstatus)
        : QTreeWidgetItem(parent),
        last_child_(0), root_(parent)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(TreeView* parent, QTreeWidgetItem* after,
                           LinkStatus const* linkstatus)
        : QTreeWidgetItem(parent, after),
        last_child_(0), root_(parent)
{
    init(linkstatus);
}

TreeViewItem::TreeViewItem(TreeView* root, QTreeWidgetItem* listview_item, QTreeWidgetItem* after,
                           LinkStatus const* linkstatus)
        : QTreeWidgetItem(listview_item, after),
        last_child_(0), root_(root)
{
    init(linkstatus);
}

TreeViewItem::~TreeViewItem()
{}

void TreeViewItem::init(LinkStatus const* linkstatus)
{
    setExpanded(true);

    for(int i = 0; i != root_->numberOfColumns(); ++i)
    {
        TreeColumnViewItem item(root_, linkstatus, i + 1);
        column_items_.push_back(item);

        QString text(KCharsets::resolveEntities(item.text(i + 1)));

        if(i + 1 == root_->urlColumnIndex()) {
            setText(item.columnIndex() - 1, QUrl::fromPercentEncoding(text.toUtf8()));
            setStatusTip(i, QTreeWidgetItem::text(i));
        }
        else if(i + 1 == root_->statusColumnIndex()) {
            setText(item.columnIndex() - 1, i18n(text.toUtf8()));
            setStatusTip(i, linkstatus->statusText());
        }
        else {
            setText(item.columnIndex() - 1, text);
            setStatusTip(i, QTreeWidgetItem::text(i));
        }

        setIcon(item.columnIndex() - 1, item.pixmap(i + 1));
        setForeground(item.columnIndex() - 1, QBrush(item.textStatusColor()));
    }
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

LinkStatus const* TreeViewItem::linkStatus() const
{
    return column_items_[0].linkStatus();
}
/*
void TreeViewItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
{
    TreeColumnViewItem item = column_items_[column];

    // Get a color to draw the text
    QColorGroup m_cg(cg);
    QColor color(item.textStatusColor());
    m_cg.setColor(QPalette::Text, color);

    QTreeWidgetItem::paintCell(p, m_cg, column, width, align);

    setHeight(22);
}
*/

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
        return QString();
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
            return SmallIcon("flag-red");
        else if(linkStatus()->status() == LinkStatus::HTTP_CLIENT_ERROR)
            return SmallIcon("flag-red");
        else if(linkStatus()->status() == LinkStatus::HTTP_REDIRECTION)
        {
            if(linkStatus()->statusText() == "304")
                return SmallIcon("ok");
            else
                return SmallIcon("edit-redo");
        }
        else if(linkStatus()->status() == LinkStatus::HTTP_SERVER_ERROR)
            return SmallIcon("flag-red");
        else if(linkStatus()->status() == LinkStatus::MALFORMED)
            // return SmallIcon("edit-delete");
          return SmallIcon("media-scripts");
        else if(linkStatus()->status() == LinkStatus::NOT_SUPPORTED)
            return SmallIcon("help-contents");
        else if(linkStatus()->status() == LinkStatus::SUCCESSFULL)
            return SmallIcon("ok");
        else if(linkStatus()->status() == LinkStatus::TIMEOUT)
            // return SmallIcon("history-clear"); 
            return SmallIcon("chronometer");
        else if(linkStatus()->status() == LinkStatus::UNDETERMINED)
            return SmallIcon("help-contents");
    }
    else if(column == root_->markupColumnIndex())
    {
        if(linkStatus()->hasHtmlErrors())
            return SmallIcon("no");
        else if(linkStatus()->hasHtmlWarnings())
            return SmallIcon("pencil");
    }

    return QPixmap();
}


#include "treeview.moc"
