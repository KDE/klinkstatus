//
// C++ Interface: resultview
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
    enum Status {
        good,
        bad,
        malformed,
        undetermined // timeouts and refs
    };

    ResultView(int column_index_status = 2,
               int column_index_label = 3,
               int column_index_URL = 1);
    virtual ~ResultView();

    virtual void insertResult(LinkStatus const* linkstatus) = 0;
    virtual void clear() = 0;
    virtual void show(Status const& status) = 0;
    virtual void showAll() = 0;
    virtual void ensureCellVisible(int row, int col) = 0;

    virtual void setColumns(QStringList const& columns);
    static bool displayableWithStatus(LinkStatus const* ls, Status const& status);

protected:
    virtual bool textFitsInCell(int row, int col) const = 0;
    virtual bool isEmpty() const = 0;
    virtual void loadContextTableMenu(QValueVector<KURL> const& referrers, bool is_root = false) = 0;

protected slots:

    virtual void slotPopupContextMenu(int row, int col, const QPoint& pos) = 0;
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
    uint col_status_;
    uint col_label_;
    uint col_url_;
    QPopupMenu context_table_menu_;
    QPopupMenu* sub_menu_;
    CellToolTip* cell_tip_;
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
    LinkStatus const* const linkStatus() const;

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
