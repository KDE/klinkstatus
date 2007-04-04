//
// C++ Interface: treeview
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TREEVIEW_H
#define TREEVIEW_H


#include "resultview.h"
#include "../engine/linkstatushelper.h"

#include <QTreeWidget>
#include <QPixmap>
#include <QResizeEvent>
class TreeViewItem;
class TreeColumnViewItem;
class LinkMatcher;

/**
@author Paulo Moura Guedes
TreeView and TreeViewItem and currently a little messes up in its API
because of ResultView. ResultView class was to be the base interface to
a QTable and a QListView, but the APIs are a little diferent... then I realize 
that a QTable view isn't needed at all so some day I will clean this up.
*/
class TreeView : public QTreeWidget, public ResultView
{
    Q_OBJECT
public:
    
    TreeView(QWidget *parent = 0);
    ~TreeView();

    QList<LinkStatus*> getVisibleLinks() const;
    QList<LinkStatus*> getBrokenLinks() const;

    virtual void setColumns(QStringList const& columns);
    virtual void clear();
    void removeColunas();
    void show(LinkMatcher const& link_matcher);
    virtual void showAll();
    
    void setTreeDisplay(bool tree_display);

    /**
    If tree_display is false the view scrolls to follow the last link inserted, 
    except if the user scrolls the view up (like Konsole).
    If tree_view, it follows always the last link inserted.
     */
    void ensureRowVisible(const QTreeWidgetItem * i, bool tree_display);
    virtual bool isEmpty() const;

signals:
    void signalLinkRecheck(LinkStatus* ls);
  
protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:

    void slotItemClicked(QTreeWidgetItem*, int column);
    void slotCustomContextMenuRequested(const QPoint&);
    void slotPopupContextMenu(QTreeWidgetItem*, QPoint const&);
    virtual void slotCopyUrlToClipboard() const;
    virtual void slotCopyParentUrlToClipboard() const;
    virtual void slotCopyCellTextToClipboard() const;
    virtual void slotEditReferrers();
    virtual void slotEditReferrer(QAction* action);
    virtual void slotViewUrlInBrowser();
    virtual void slotViewParentUrlInBrowser();
    virtual void loadContextTableMenu(QSet<KUrl> const& referrers, bool is_root = false);
    virtual void slotRecheckUrl();

private:
    void resetColumns();
    double columnsWidth() const;
    bool isVisible(QTreeWidgetItem* item, LinkMatcher const& link_matcher) const;
    void setItemVisibleRecursively(QTreeWidgetItem* item, LinkMatcher const& link_matcher);
    void setItemVisibleRecursively(QTreeWidgetItem* item, bool hidden);

    static TreeViewItem* myItem(QTreeWidgetItem* item);
    QList<LinkStatus*> getLinksWithCriteria(LinkMatcher const& link_matcher) const;
    void addLinksWithCriteriaRecursively(QList<LinkStatus*>& items, TreeViewItem* item, LinkMatcher const& link_matcher) const;
    void addVisibleItemsRecursively(QList<LinkStatus*>& items, TreeViewItem* item) const;

private:
    int current_column_; // apparently it's impossible to know what is the current column
    bool tree_display_;
};

inline void TreeView::setTreeDisplay(bool tree_display) { 
    tree_display_ = tree_display; 
    setRootIsDecorated(tree_display_);
}


/* ******************************* TreeViewItem ******************************* */

class TreeViewItem: public QTreeWidgetItem
{
public:

    TreeViewItem(TreeView* parent, LinkStatus* linkstatus);
    TreeViewItem(TreeView* parent, QTreeWidgetItem* after,
                 LinkStatus* linkstatus);
    TreeViewItem(TreeView* root, QTreeWidgetItem* parent_item, QTreeWidgetItem* after,
                 LinkStatus* linkstatus);
    virtual ~TreeViewItem();

    void refresh(LinkStatus*);

    void setLastChild(QTreeWidgetItem* last_child);
    QTreeWidgetItem* lastChild() const;
    
    QString key(int column, bool) const;
    LinkStatus* linkStatus() const;

  private:
    void init(LinkStatus* linkstatus);

private:
    Q3ValueList<TreeColumnViewItem> column_items_;
    QTreeWidgetItem* last_child_;
    TreeView* root_;
};


/* ******************************* TreeColumnViewItem ******************************* */

class TreeColumnViewItem
{
public:
    TreeColumnViewItem()
    {}
    ;
    TreeColumnViewItem(TreeView* root, LinkStatus* linkstatus, int column_index);
    ~TreeColumnViewItem();

    //void setColumnIndex(int i);
    int columnIndex() const;
    LinkStatus* linkStatus() const;
    QColor const textStatusColor() const;
    QString text(int column) const;
    QPixmap pixmap(int column) const;

private:
    TreeView* root_;
    LinkStatus* ls_;
    int column_index_;
};

#endif
