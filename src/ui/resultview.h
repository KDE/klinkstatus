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

#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include <qvaluevector.h>
#include <qpopupmenu.h>
#include <qstringlist.h>

class KURL;

class LinkStatus;
class CellToolTip;


/**
@author Paulo Moura Guedes
*/
class ResultView
{
public:
    
    static const QString URL_LABEL;
    static const QString STATUS_LABEL;
    static const QString MARKUP_LABEL;
    static const QString LINK_LABEL_LABEL;
    
    enum Status {
        none = 0,
        good,
        bad,
        malformed,
        undetermined // timeouts and refs
    };

    ResultView();
    virtual ~ResultView();

    //virtual void insertResult(LinkStatus const* linkstatus) = 0;
    virtual void clear() = 0;
    virtual void show(Status const& status) = 0;
    virtual void showAll() = 0;
    //virtual void ensureCellVisible(int row, int col) = 0;

    virtual void setColumns(QStringList const& columns);
    static bool displayableWithStatus(LinkStatus const* ls, Status const& status);
    
    int numberOfColumns() const { return number_of_columns_; }
    
    int urlColumnIndex() const {return col_url_; }
    int statusColumnIndex() const {return col_status_; }
    int markupColumnIndex() const {return col_markup_; }
    int labelColumnIndex() const {return col_label_; }

protected:
    //virtual bool textFitsInCell(int row, int col) const = 0;
    virtual bool isEmpty() const = 0;
    virtual void loadContextTableMenu(QValueVector<KURL> const& referrers, bool is_root = false) = 0;

protected slots:

    //virtual void slotPopupContextMenu(int row, int col, const QPoint& pos) = 0;
    virtual void slotCopyUrlToClipboard() const = 0;
    virtual void slotCopyParentUrlToClipboard() const = 0;
    virtual void slotCopyCellTextToClipboard() const = 0;
    virtual void slotEditReferrersWithQuanta() = 0;
    virtual void slotEditReferrerWithQuanta(int id) = 0;
    virtual void slotEditReferrerWithQuanta(KURL const& url) = 0;
    virtual void slotViewUrlInBrowser() = 0;
    virtual void slotViewParentUrlInBrowser() = 0;

protected:
    QStringList columns_;
    int col_status_;
    int col_label_;
    int col_url_;
    int col_markup_; // optional
    QPopupMenu context_table_menu_;
    QPopupMenu* sub_menu_;
    CellToolTip* cell_tip_;

private:
    int number_of_columns_;
};


class ResultViewItem
{
public:
    ResultViewItem(LinkStatus const* linkstatus,
                   int column_index);
    virtual ~ResultViewItem();
    
    virtual void setColumnIndex(int i);
    virtual int columnIndex() const;

    virtual QString toolTip() const = 0;
    LinkStatus const* linkStatus() const;

protected:

    QColor const& textStatusColor() const;
    virtual void paint( QPainter *p, const QColorGroup &cg,
                        const QRect &cr, bool selected ) = 0;
    virtual void setText() = 0;
    virtual void setPixmap() = 0;

protected:

    LinkStatus* ls_;
    int column_index_;
    int alignment_;
};


#endif
