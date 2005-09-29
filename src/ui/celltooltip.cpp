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

#include "celltooltip.h"
#include "tablelinkstatus.h"

#include <qscrollview.h>

#include <iostream>
using namespace std;


CellToolTip::CellToolTip ( TableLinkstatus * table, QToolTipGroup * group)
        : QToolTip(table->viewport(), group), table_(table)
{}

void CellToolTip::maybeTip ( const QPoint & p )
{
    QPoint cp =  table_->viewportToContents(p);

    int row = table_->rowAt(cp.y());
    int col = table_->columnAt(cp.x());

    if( row != -1 && col != -1)
    {
        if(col == 0 || !table_->textFitsInCell(row, col))
        {
            TableItem* item = table_->myItem(row, col);
            QString tip_string = item->toolTip();

            QRect cr = table_->cellGeometry( row, col );
            cr.moveTopLeft( table_->contentsToViewport( cr.topLeft() ) );

            tip(cr, tip_string);
        }
    }
}
