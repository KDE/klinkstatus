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

#ifndef TABLE_LINKSTATUS_H
#define TABLE_LINKSTATUS_H

#include <qtable.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qvaluevector.h>
class QStringList;

class KURL;

#include <vector>

#include "../engine/linkstatus.h"
#include "celltooltip.h"
#include "resultview.h"

using namespace std;


int const STATUS_COLUMN_WIDTH = 50;

class TableItem;

class TableLinkstatus: public QTable, public ResultView
{
    Q_OBJECT
public:

    TableLinkstatus(QWidget * parent = 0, const char * name = 0,
                    int column_index_status = 1,
                    int column_index_label = 2,
                    int column_index_URL = 3);
    ~TableLinkstatus();

    virtual void setColumns(QStringList const& columns);

    /* Insere uma nova entrada no fim da tabela */
    virtual void insertResult(LinkStatus const* linkstatus);


    virtual void clear();
    void removeColunas();
    virtual void show(ResultView::Status const& status);
    virtual void showAll();


    /* Specialization of QTable::ensureCellVisible */
    virtual void ensureCellVisible(int row, int col);

    virtual bool textFitsInCell(int row, int col) const;
    virtual bool isEmpty() const;

    TableItem* myItem(int row, int col) const;

private slots:

    virtual void slotPopupContextMenu(int row, int col, const QPoint& pos);
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

    vector<TableItem*> generateRowOfTableItems(LinkStatus const* linkstatus);
    void insereLinha(vector<TableItem*> items);

private:
/*
    int col_status_;
    int col_label_;
    int col_url_;
    CellToolTip* cell_tip_;
    QPopupMenu context_table_menu_;
    QPopupMenu* sub_menu_;
*/
};


class TableItem: public QTableItem, public ResultViewItem
{
public:

    TableItem(QTable* table, EditType et,
              LinkStatus const* linkstatus,
              int column_index, int alignment = Qt::AlignLeft);
    virtual ~TableItem();

    virtual void setColumnIndex(int i);
    virtual int columnIndex() const;

    void setAlignment(int aFlags);
    virtual int alignment() const;

    virtual QString toolTip() const = 0;
    //LinkStatus const* const linkStatus() const;

protected:

    //QColor const& textStatusColor() const;
    virtual void paint( QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected );
    virtual void setText() = 0;
    virtual void setPixmap() = 0;

private:

    //LinkStatus* ls_;
    //int column_index_;
    int alignment_;
};


class TableItemURL: public TableItem
{
public:

    TableItemURL(QTable* table, EditType et,
                 LinkStatus const* linkstatus, int column_index = 3);
    //virtual ~TableItemURL(){};

    virtual QString toolTip() const;

protected:

    virtual void setText();
    virtual void setPixmap();
    virtual void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
    QColor const& textStatusColor() const;
};


class TableItemStatus: public TableItem
{
public:

    TableItemStatus(QTable* table, EditType et,
                    LinkStatus const* linkstatus, int column_index = 1);
    //virtual ~TableItemStatus(){};

    virtual QString toolTip() const;

protected:

    virtual void setText();
    virtual void setPixmap();
    virtual void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
};


class TableItemNome: public TableItem
{
public:

    TableItemNome(QTable* table, EditType et,
                  LinkStatus const* linkstatus, int column_index = 2);
    //virtual ~TableItemNome(){};

    virtual QString toolTip() const;

protected:

    virtual void setText();
    virtual void setPixmap();
    //virtual void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
};


inline void TableItem::paint( QPainter *p, const QColorGroup &cg,
                              const QRect &cr, bool selected )
{
    QTableItem::paint(p, cg, cr, selected);
}

#endif
