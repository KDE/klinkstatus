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

#include <k3listview.h>

#include "resultview.h"
//Added by qt3to4:
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
class TreeView : public K3ListView, public ResultView
{
    Q_OBJECT
public:
    
    TreeView(QWidget *parent = 0, const char *name = 0);
    ~TreeView();

    virtual void setColumns(QStringList const& columns);
    virtual void clear();
    void removeColunas();
    virtual void show(ResultView::Status const& status);
    void show(LinkMatcher link_matcher);
    virtual void showAll();
    
    void setTreeDisplay(bool tree_display);

    /**
    If tree_display is false the view scrolls to follow the last link inserted, 
    except if the user scrolls the view up (like Konsole).
    If tree_view, it follows always the last link inserted.
     */
    void ensureRowVisible(const Q3ListViewItem * i, bool tree_display);
    virtual bool isEmpty() const;

    TreeViewItem* myItem(Q3ListViewItem* item) const;
    
protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:

    void slotPopupContextMenu(Q3ListViewItem *, const QPoint &, int);    
    virtual void slotCopyUrlToClipboard() const;
    virtual void slotCopyParentUrlToClipboard() const;
    virtual void slotCopyCellTextToClipboard() const;
    virtual void slotEditReferrersWithQuanta();
    virtual void slotEditReferrerWithQuanta(int id);
    virtual void slotEditReferrerWithQuanta(KUrl const& url);
    virtual void slotViewUrlInBrowser();
    virtual void slotViewParentUrlInBrowser();
    virtual void loadContextTableMenu(Q3ValueVector<KUrl> const& referrers, bool is_root = false);

private:
    void resetColumns();
    double columnsWidth() const;

private:
    int current_column_; // apparently it's impossible to know what is the current column
    bool tree_display_;
};

inline void TreeView::setTreeDisplay(bool tree_display) { 
    tree_display_ = tree_display; 
    setRootIsDecorated(tree_display_);
}


/* ******************************* TreeViewItem ******************************* */

class TreeViewItem: public K3ListViewItem
{
public:

    TreeViewItem(TreeView* parent, Q3ListViewItem* after,
                 LinkStatus const* linkstatus);
    TreeViewItem(TreeView* root, Q3ListViewItem* parent_item, Q3ListViewItem* after,
                 LinkStatus const* linkstatus);
    virtual ~TreeViewItem();

    void setLastChild(Q3ListViewItem* last_child);
    Q3ListViewItem* lastChild() const;
    
    QString key(int column, bool) const;
    LinkStatus const* linkStatus() const;

protected:
    virtual void paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align);

private:
    void init(LinkStatus const* linkstatus);

private:
    Q3ValueVector<TreeColumnViewItem> column_items_;
    Q3ListViewItem* last_child_;
    TreeView* root_;
};


/* ******************************* TreeColumnViewItem ******************************* */

class TreeColumnViewItem
{
public:
    TreeColumnViewItem()
    {}
    ;
    TreeColumnViewItem(TreeView* root, LinkStatus const* linkstatus, int column_index);
    ~TreeColumnViewItem();

    //void setColumnIndex(int i);
    int columnIndex() const;
    LinkStatus const* linkStatus() const;
    QColor const& textStatusColor() const;
    QString text(int column) const;
    QPixmap pixmap(int column) const;

private:
    TreeView* root_;
    LinkStatus const* ls_;
    int column_index_;
};

#endif
