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

#include <klistview.h>

#include "resultview.h"
class TreeViewItem;
class TreeColumnViewItem;

/**
@author Paulo Moura Guedes
TreeView and TreeViewItem and currently a little messes up in its API
because of ResultView. ResultView class was to be the base interface to
a QTable and a QListView, but the APIs are a little diferent... then I realize 
that a QTable view isn't needed at all so some day I will clean this up.
*/
class TreeView : public KListView, public ResultView
{
    Q_OBJECT
public:
    TreeView(QWidget *parent = 0, const char *name = 0,
             int column_index_status = 2,
             int column_index_label = 3,
            int column_index_URL = 1);
    ~TreeView();

    virtual void setColumns(QStringList const& columns);
    virtual void clear();
    void removeColunas();
    virtual void show(ResultView::Status const& status);
    virtual void showAll();

    /**
    If tree_display is false the view scrolls to follow the last link inserted, 
    except if the user scrolls the view up (like Konsole).
    If tree_view, it follows always the last link inserted.
     */
    void ensureRowVisible(const QListViewItem * i, bool tree_display);
    virtual bool isEmpty() const;

    TreeViewItem* myItem(QListViewItem* item) const;
    
protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:

    void slotPopupContextMenu(QListViewItem *, const QPoint &, int);    
    virtual void slotCopyUrlToClipboard() const;
    virtual void slotCopyParentUrlToClipboard() const;
    virtual void slotCopyCellTextToClipboard() const;
    virtual void slotEditReferrersWithQuanta();
    virtual void slotEditReferrerWithQuanta(int id);
    virtual void slotEditReferrerWithQuanta(KURL const& url);
    virtual void slotViewUrlInBrowser();
    virtual void slotViewParentUrlInBrowser();
    virtual void loadContextTableMenu(QValueVector<KURL> const& referrers, bool is_root = false);

private:
    void resetColumns();

private:
    int current_column_; // apparently it's impossible to know what is the current column
};


/* ******************************* TreeViewItem ******************************* */

class TreeViewItem: public KListViewItem
{
public:

    TreeViewItem(QListView* listview, QListViewItem* after,
                 LinkStatus const* linkstatus, int number_of_columns);
    TreeViewItem(QListViewItem* listview_item, QListViewItem* after,
                 LinkStatus const* linkstatus, int number_of_columns);
    virtual ~TreeViewItem();

    void setLastChild(QListViewItem* last_child);
    QListViewItem* lastChild() const;

    QString key(int column, bool) const;
    LinkStatus const* const linkStatus() const;

protected:
    virtual void paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align);

private:
    void init(LinkStatus const* linkstatus);

private:
    QValueVector<TreeColumnViewItem> column_items_;
    int const number_of_columns_;
    QListViewItem* last_child_;
};


/* ******************************* TreeColumnViewItem ******************************* */

class TreeColumnViewItem
{
public:
    TreeColumnViewItem()
    {}
    ;
    TreeColumnViewItem(LinkStatus const* linkstatus, int column_index);
    ~TreeColumnViewItem();

    //void setColumnIndex(int i);
    int columnIndex() const;
    LinkStatus const* const linkStatus() const;
    QColor const& textStatusColor() const;
    QString text(int column) const;
    QPixmap pixmap(int column) const;

private:
    LinkStatus const* ls_;
    int column_index_;
};

#endif
