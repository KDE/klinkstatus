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

#include "tablelinkstatus.h"
#include "../utils/utils.h"
#include "../parser/url.h"

#include <qmemarray.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qprocess.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <krun.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <cassert>
#include <iostream>


/*
 
********************* TableLinkstatus ***************************
 
*/

TableLinkstatus::TableLinkstatus(QWidget * parent, const char * name,
                                 int column_index_status,
                                 int column_index_label,
                                 int column_index_URL)
        : QTable(parent, name),
        col_status_(column_index_status),
        col_label_(column_index_label),
        col_url_(column_index_URL),
        context_table_menu_(this, "context_table_menu")
{
    setShowGrid(false);
    setSorting(false);
    setSelectionMode(QTable::NoSelection);
    setFocusStyle(QTable::FollowStyle);
    setReadOnly(true);

    verticalHeader()->hide();
    setLeftMargin(0);

    cell_tip_ = new CellToolTip(this);
    loadContextTableMenu();
}

TableLinkstatus::~TableLinkstatus()
{
    delete cell_tip_;
}

void TableLinkstatus::setColunas(vector<QString> const& columns)
{
    assert(columns.size() != 0);

    removeColunas();
    setNumCols(columns.size());

    QHeader* horizontal_header = horizontalHeader();
    for(vector<QString>::size_type i = 0; i != columns.size(); ++i)
    {
        if(i == 0)
        {
            assert(columns[i] == i18n("Status") && col_status_ == 1);
            setColumnWidth(i, STATUS_COLUMN_WIDTH);
        }
        else if(i == 1)
        {
            assert(columns[i] == i18n("Label") && col_label_ == 2);
            setColumnWidth(i, width() / 3);
        }
        else if(i == 2)
            assert(columns[i] == i18n("URL") && col_url_ == 3);

        horizontal_header->setLabel(i, i18n(columns[i]));
    }

    setColumnStretchable(col_url_ - 1, true);
    horizontal_header->adjustHeaderSize();
}

void TableLinkstatus::insereLinha(vector<TableItem*> items)
{
    assert(items.size() == (uint)numCols());

    setNumRows(numRows() + 1);
    int row = numRows() - 1;

    for(vector<TableItem*>::size_type i = 0; i != items.size(); ++i)
    {
        assert(items[i]);

        int col = items[i]->columnIndex() - 1;
        setItem(row, col, items[i]);
    }

    if(items[col_url_ - 1]->sizeHint().width() > columnWidth(col_url_ - 1))
    {
        setColumnStretchable(col_url_ - 1, false);
        setColumnWidth(col_url_ - 1, items[col_url_ - 1]->sizeHint().width());
    }

    ensureCellVisible ( row, 0 );
}

void TableLinkstatus::removeLinhas()
{
    QMemArray<int> linhas(numRows());
    for(uint i = 0; i != linhas.size(); ++i)
        linhas[i] = i + 1;

    removeRows(linhas);

    assert(numRows() == 0);
}

void TableLinkstatus::removeColunas()
{
    QMemArray<int> columns(numCols());
    for(uint i = 0; i != columns.size(); ++i)
        columns[i] = i + 1;

    removeColumns(columns);

    assert(numCols() == 0);
}

void TableLinkstatus::mostraPorStatusCode(int status_code)
{
    for(int i = 0; i != numRows(); ++i)
    {
        int row = i + 1;
        QTableItem* _item = myItem(row, col_status_);

        if(status_code != _item->text().toInt())
            hideRow(row);
    }
}

/**
   Use this procedure when you insert a row at the bottom of the table, 
   and you only want the to scroll down if you were already at the bottom, 
   before inserting the row.
   This allows you to see what's going on on other cells without having 
   the table always scrolling down when every row is inserted.
*/
void TableLinkstatus::ensureCellVisible(int row, int col)
{
    // table viewport is at the bottom
    if( rowPos(row - 1) <= (contentsY() + visibleHeight()) )
        QTable::ensureCellVisible(row, col);
}

bool TableLinkstatus::textoCabeNaCelula(int row, int col) const
{
    QTableItem* itm(myItem(row, col));
    assert(itm);

    QSize size_hint(itm->sizeHint());

    if(columnWidth(col) < size_hint.width())
        return false;
    else
        return true;
}

bool TableLinkstatus::isEmpty() const
{
    return numRows() == 0;
}

TableItem* TableLinkstatus::myItem(int row, int col) const
{
    TableItem* _item = dynamic_cast<TableItem*> (QTable::item(row, col));
    //assert(_item);
    return _item;
}

void TableLinkstatus::slotPopupContextMenu(int r, int w, const QPoint& pos)
{
    if(myItem(r, w))
        context_table_menu_.popup(pos);
}

void TableLinkstatus::loadContextTableMenu()
{
    connect(this, SIGNAL( contextMenuRequested ( int, int, const QPoint&  )),
            this, SLOT( slotPopupContextMenu( int, int, const QPoint&)) );

    context_table_menu_.insertItem(SmallIconSet("fileopen"), i18n("Open URL"),
                                   this, SLOT(slotViewUrlInBrowser()));

    context_table_menu_.insertItem(SmallIconSet("fileopen"), i18n("Open Parent URL"),
                                   this, SLOT(slotViewParentUrlInBrowser()));

    context_table_menu_.insertSeparator();

    context_table_menu_.insertItem(SmallIconSet("editcopy"), i18n("Copy URL"),
                                   this, SLOT(slotCopyUrlToClipboard()));

    context_table_menu_.insertItem(SmallIconSet("editcopy"), i18n("Copy Parent URL"),
                                   this, SLOT(slotCopyParentUrlToClipboard()));

    context_table_menu_.insertItem(SmallIconSet("editcopy"), i18n("Copy cell text"),
                                   this, SLOT(slotCopyCellTextToClipboard()));
}

void TableLinkstatus::slotCopyUrlToClipboard() const
{
    TableItem* _item = myItem(currentRow(), currentColumn());
    QString content(_item->linkStatus()->absoluteUrl().prettyURL());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TableLinkstatus::slotCopyParentUrlToClipboard() const
{
    TableItem* _item = myItem(currentRow(), currentColumn());
    QString content(_item->linkStatus()->parent()->absoluteUrl().prettyURL());
    QClipboard* cb = kapp->clipboard();
    cb->setText(content);
}

void TableLinkstatus::slotCopyCellTextToClipboard() const
{
    QString cell_text(text(currentRow(), currentColumn()));
    QClipboard* cb = kapp->clipboard();
    cb->setText(cell_text);
}

void TableLinkstatus::slotViewUrlInBrowser()
{
    TableItem* _item = myItem(currentRow(), currentColumn());
    KURL url = _item->linkStatus()->absoluteUrl();

    if(url.isValid())
    {
        (void) new KRun (url, 0, url.isLocalFile(), true);
    }
    else
        KMessageBox::sorry(this, i18n("Invalid URL."));
}

void TableLinkstatus::slotViewParentUrlInBrowser()
{
    TableItem* _item = myItem(currentRow(), currentColumn());

    if(_item->linkStatus()->isRoot())
    {
        KMessageBox::sorry(this, i18n("ROOT URL."));
    }

    else
    {
        LinkStatus const* ls_parent = _item->linkStatus()->parent();
        assert(ls_parent);

        KURL url = ls_parent->absoluteUrl();

        if(url.isValid())
            (void) new KRun (url, 0, url.isLocalFile(), true);
        else
            KMessageBox::sorry(this, i18n("Invalid URL."));
    }
}

/*
 
********************* TableItem ***************************
 
*/

TableItem::TableItem(QTable* table, EditType et,
                     LinkStatus const* linkstatus,
                     int column_index, int alignment)
        : QTableItem(table, et, ""), ls_((LinkStatus*)linkstatus),
        column_index_(column_index), alignment_(alignment)
{
    assert(ls_);
    assert(column_index_ > 0);
}

TableItem::~TableItem()
{}

void TableItem::setColumnIndex(int i)
{
    assert(i > 0 && i <= table()->numCols());

    column_index_ = i;
}

int TableItem::columnIndex() const
{
    assert(column_index_ <= table()->numCols());

    return column_index_;
}

void TableItem::setAlignment(int aFlags)
{
    alignment_ = aFlags;
}

int TableItem::alignment() const
{
    return alignment_;
}

LinkStatus const* const TableItem::linkStatus() const
{
    return ls_;
}

QColor const& TableItem::textStatusColor() const
{
    if(linkStatus()->errorOccurred())
    {
        //kdDebug(23100) <<  "ERROR: " << linkStatus()->error() << ": " << linkStatus()->absoluteUrl().prettyURL() << endl;
        if(linkStatus()->error() == "Javascript not suported")
            return lightGray;
        else
            return red;
    }

    else if(linkStatus()->absoluteUrl().protocol() != "http" &&
            linkStatus()->absoluteUrl().protocol() != "https")
        return darkGreen;

    else
    {
        QString status_code(QString::number(linkStatus()->httpHeader().statusCode()));

        if(status_code[0] == '0')
        {
            kdDebug(23100) <<  "statuc code == 0: " << endl;
            kdDebug(23100) <<  linkStatus()->toString() << endl;
            kdDebug(23100) <<  linkStatus()->httpHeader().toString() << endl;
        }
        //assert(status_code[0] != '0');

        if(status_code[0] == '5')
            return darkMagenta;

        else if(status_code[0] == '4')
            return red;

        else if(status_code[0] == '3')
            return blue;

        else if(status_code[0] == '2')
            return darkGreen;

        else
            return red;
    }
}


/*
 
********************* TableItemURL ***************************
 
*/

TableItemURL::TableItemURL(QTable* table, EditType et,
                           LinkStatus const* linkstatus, int column_index)
        : TableItem(table, et, linkstatus, column_index)
{
    setText();
}

void TableItemURL::setText()
{
    if(linkStatus()->node() && linkStatus()->malformed())
    {
        if(linkStatus()->node()->url().isEmpty())
            QTableItem::setText( linkStatus()->node()->content().simplifyWhiteSpace() );
        else
            QTableItem::setText( linkStatus()->node()->url() );
    }

    else
    {
        KURL url = linkStatus()->absoluteUrl();

        if(url.protocol() == "http" || url.protocol() == "https")
        {
            if(linkStatus()->isLocalRestrict())
            {
                QTableItem::setText(::convertToLocal(url));
            }
            else
                QTableItem::setText(linkStatus()->absoluteUrl().prettyURL());
        }
        else
            QTableItem::setText(linkStatus()->absoluteUrl().prettyURL());
    }
}

void TableItemURL::setPixmap()
{}

QString TableItemURL::toolTip() const
{
    return text(); // Pode parecer repeticao mas nao eh... Ver construtor
}

void TableItemURL::paint( QPainter *p, const QColorGroup &cg,
                          const QRect &cr, bool selected )
{
    // Get a color to draw the text
    QColorGroup m_cg(cg);
    QColor color(textStatusColor());
    m_cg.setColor(QColorGroup::Text, color);

    QTableItem::paint(p, m_cg, cr, selected);
}

QColor const& TableItemURL::textStatusColor() const
{
    // TODO clean this code

    QString status_code(QString::number(linkStatus()->httpHeader().statusCode()));

    if(linkStatus()->errorOccurred())
    {
        if(linkStatus()->error().contains("Timeout"))
            return darkMagenta;
        else if(linkStatus()->error().contains("not suported"))
            return lightGray;
        else
            return red;
    }

    else if(linkStatus()->absoluteUrl().protocol() != "http" &&
            linkStatus()->absoluteUrl().protocol() != "https")
        return black;

    else if(status_code[0] == '5')
        return darkMagenta;

    else if(status_code[0] == '4')
        return red;

    else
        return black;
}

/*
 
********************* TableItemStatus ***************************
 
*/

TableItemStatus::TableItemStatus(QTable* table, EditType et,
                                 LinkStatus const* linkstatus, int column_index)
        : TableItem(table, et, linkstatus, column_index)
{
    setAlignment(Qt::AlignHCenter /*| Qt :: AlignVCenter*/);
    setText();
    setPixmap();
}

void TableItemStatus::setText()
{
    if(linkStatus()->errorOccurred() || linkStatus()->status() == "OK")
    {
        QTableItem::setText("");
    }

    else
    {
        /*
                if(linkStatus()->httpHeader().statusCode() == 0)
                {
                    kdDebug(23100) <<  "TableItemStatus::setText : statusCode() == 0" << endl;
                    kdDebug(23100) <<  linkStatus()->toString() << endl;
                    kdDebug(23100) <<  linkStatus()->docHtml() << endl;
                }
        */
        //assert(linkStatus()->httpHeader().statusCode() != 0); //<------------------------------------------------------------
        //QTableItem::setText( QString::number(linkStatus()->httpHeader().statusCode()) );
        QTableItem::setText( linkStatus()->status() );
    }
}

void TableItemStatus::setPixmap()
{
    if(linkStatus()->errorOccurred())
    {

        if(linkStatus()->error().contains("Timeout"))
        {
            QTableItem::setPixmap(SmallIcon("kalarm"));
        }
        else if(linkStatus()->error() == "Malformed")
        {
            QTableItem::setPixmap(locate("data", "khtml/icons/crystalsvg/16x16/actions/bug.png"));
        }
        else
        {
            QTableItem::setPixmap(SmallIcon("no"));
        }
    }
    else if(linkStatus()->status() == "OK")
        QTableItem::setPixmap(SmallIcon("ok"));
}

QString TableItemStatus::toolTip() const
{
    if(linkStatus()->errorOccurred() ||
            (linkStatus()->absoluteUrl().protocol() != "http" &&
             linkStatus()->absoluteUrl().protocol() != "https"))
    {
        return i18n("%1").arg(linkStatus()->status());
    }

    else
        return i18n("%1").arg(linkStatus()->httpHeader().reasonPhrase());
}

void TableItemStatus::paint( QPainter *p, const QColorGroup &cg,
                             const QRect &cr, bool selected )
{
    p->fillRect( 0, 0, cr.width(), cr.height(),
                 selected ? cg.brush( QColorGroup::Highlight )
                 : cg.brush( QColorGroup::Base ) );

    int w = cr.width();
    int h = cr.height();

    int x = 0;
    if ( !pixmap().isNull() )
    {
        p->drawPixmap( ( w - pixmap().width() ) / 2,
                       ( h - pixmap().height() ) / 2,
                       pixmap() );
        x = pixmap().width() + 2;
    }

    // Get a color to draw the text
    QColorGroup m_cg(cg);
    QColor color(textStatusColor());
    m_cg.setColor(QColorGroup::Text, color);

    //QTableItem::paint(p, m_cg, cr, selected);

    if ( selected )
        p->setPen( m_cg.highlightedText() );
    else
        p->setPen( m_cg.text() );
    p->drawText( x + 2, 0, w - x - 4, h,
                 wordWrap() ? (alignment() | WordBreak) : alignment(), text() );
}

/*
 
********************* TableItemNome ***************************
 
*/

TableItemNome::TableItemNome(QTable* table, EditType et,
                             LinkStatus const* linkstatus, int column_index)
        : TableItem(table, et, linkstatus, column_index)
{
    setText();
}

void TableItemNome::setText()
{
    QString label(linkStatus()->label());
    if(!label.isNull())
        QTableItem::setText(label.simplifyWhiteSpace());
}

void TableItemNome::setPixmap()
{}

QString TableItemNome::toolTip() const
{
    return text(); // Pode parecer repeticao mas nao eh... Ver construtor
}

#include "tablelinkstatus.moc"
