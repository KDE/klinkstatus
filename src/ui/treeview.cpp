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
#include "treeview.h"

TreeView::TreeView(QWidget *parent, const char *name)
        : KListView(parent, name)
{}


TreeView::~TreeView()
{}


TreeViewItem::TreeViewItem(KListView* listview,
             LinkStatus const* linkstatus,
             int column_index)
        : KListViewItem(listview), ResultViewItem(linkstatus, column_index)
{}

TreeViewItem::TreeViewItem(KListViewItem* listview_item,
             LinkStatus const* linkstatus,
             int column_index)
        : KListViewItem(listview_item), ResultViewItem(linkstatus, column_index)
{}


#include "treeview.moc"
