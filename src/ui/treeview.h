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

#include <klistview.h>

/**
@author Paulo Moura Guedes
*/
class TreeView : public KListView, public ResultView
{
    Q_OBJECT
public:
    TreeView(QWidget *parent = 0, const char *name = 0);
    ~TreeView();

    virtual void setColumns(QStringList const& columns){}

    /* Insere uma nova entrada no fim da tabela */
    virtual void insertResult(LinkStatus const* linkstatus){}


    virtual void clear(){}
    void removeColunas(){}
    virtual void show(ResultView::Status const& status){}
    virtual void showAll(){}


    /* Specialization of QTable::ensureCellVisible */
    virtual void ensureCellVisible(int row, int col){}

    virtual bool textFitsInCell(int row, int col) const{}
    virtual bool isEmpty() const{}

    //TreeViewItem* myItem(int row, int col) const;

private slots:

    virtual void slotPopupContextMenu(int row, int col, const QPoint& pos){}
    virtual void slotCopyUrlToClipboard() const{}
    virtual void slotCopyParentUrlToClipboard() const{}
    virtual void slotCopyCellTextToClipboard() const{}
    virtual void slotEditReferrersWithQuanta(){}
    virtual void slotEditReferrerWithQuanta(int id){}
    virtual void slotEditReferrerWithQuanta(KURL const& url){}
    virtual void slotViewUrlInBrowser(){}
    virtual void slotViewParentUrlInBrowser(){}
    virtual void loadContextTableMenu(QValueVector<KURL> const& referrers){}
};



class TreeViewItem: public KListViewItem, public ResultViewItem
{
public:

    TreeViewItem(KListView* listview,
                 LinkStatus const* linkstatus,
                 int column_index);
    TreeViewItem(KListViewItem* listview_item,
                 LinkStatus const* linkstatus,
                 int column_index);
    virtual ~TreeViewItem(){}

    virtual void setColumnIndex(int i){}
    virtual int columnIndex() const{}

    virtual QString toolTip() const{}

protected:

    virtual void paint( QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected ){}
    virtual void setText() = 0;
    virtual void setPixmap() = 0;
};

#endif
