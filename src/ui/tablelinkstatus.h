/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   pmg@netcabo.pt                                                        *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TABLE_LINKSTATUS_H
#define TABLE_LINKSTATUS_H

#include <qtable.h>
#include <qstring.h>
#include <qptrvector.h>
#include <qcolor.h>
#include <qpopupmenu.h>

#include <vector>

#include "../engine/linkstatus.h"
#include "celltooltip.h"

using namespace std;


int const STATUS_COLUMN_WIDTH = 50;

class TableItem;

class TableLinkstatus: public QTable
{
    Q_OBJECT

public:

    TableLinkstatus(QWidget * parent = 0, const char * name = 0,
                    int column_index_status = 1,
                    int column_index_label = 2,
                    int column_index_URL = 3);
    ~TableLinkstatus();

    void setColunas(vector<QString> const& columns);

    /* Insere uma nova entrada no fim da tabela */
    void insereLinha(vector<TableItem*> items);

    void removeLinhas();
    void removeColunas();
    void mostraPorStatusCode(int status_code);

    /* Specialization of QTable::ensureCellVisible */
    virtual void ensureCellVisible(int row, int col);

    bool textoCabeNaCelula(int row, int col) const;
    bool isEmpty() const;

    TableItem* myItem(int row, int col) const;

private slots:

    void slotPopupContextMenu(int row, int col, const QPoint& pos);
    void slotCopyUrlToClipboard() const;
    void slotCopyParentUrlToClipboard() const;
    void slotCopyCellTextToClipboard() const;
    void slotViewUrlInBrowser();
    void slotViewParentUrlInBrowser();

private:

    void loadContextTableMenu();
    //void slotViewInBrowser(KURL const& url);

private:

    int col_status_;
    int col_label_;
    int col_url_;
    CellToolTip* cell_tip_;
    QPopupMenu context_table_menu_;

};


class TableItem: public QTableItem
{
public:

    TableItem(QTable* table, EditType et,
              LinkStatus const* linkstatus,
              int column_index, int alignment = Qt::AlignLeft);
    virtual ~TableItem();

    void setColumnIndex(int i);
    int columnIndex() const;

    void setAlignment(int aFlags);
    virtual int alignment() const;

    virtual QString toolTip() const = 0;
    LinkStatus const* const linkStatus() const;

protected:

    QColor const& textStatusColor() const;
    virtual void paint( QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected );
    virtual void setText() = 0;
    virtual void setPixmap() = 0;

private:

    LinkStatus* ls_;
    int column_index_;
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
