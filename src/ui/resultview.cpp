/***************************************************************************
 *   Copyright (C) 2004-2007 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "resultview.h"
#include "../engine/linkstatus.h"

#include <QPainter>
#include <qcolor.h>

#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>

const QString ResultView::URL_LABEL = "URL";
const QString ResultView::STATUS_LABEL = "Status";
const QString ResultView::MARKUP_LABEL = "Markup";
const QString ResultView::LINK_LABEL_LABEL = "Label";


// ******************************* ResultView ********************************

ResultView::ResultView()
        : col_status_(-1),
        col_label_(-1),
        col_url_(-1),
        col_markup_(-1),
        sub_menu_(0),
        cell_tip_(0)
{}


ResultView::~ResultView()
{}

void ResultView::setColumns(QStringList const& columns)
{
    Q_ASSERT(columns.size() != 0);
    
    columns_.clear();
    for(int i = 0; i != columns.size(); ++i)
    {
        if(columns[i] == ResultView::URL_LABEL)
        {
            col_url_ = i + 1;
        }
        else if(columns[i] == ResultView::STATUS_LABEL)
        {
            col_status_ = i + 1;
        }
        else if(columns[i] == ResultView::MARKUP_LABEL)
        {
            col_markup_ = i + 1;
        }
        else if(columns[i] == ResultView::LINK_LABEL_LABEL)
        {
            col_label_ = i + 1;
        }

        columns_.push_back(columns[i]);
    }
    number_of_columns_ = columns.size();
}

bool ResultView::displayableWithStatus(LinkStatus const* ls, Status const& status)
{
    if(status == ResultView::good)
    {
        return 
                ls->status() == LinkStatus::SUCCESSFULL ||
                ls->status() == LinkStatus::HTTP_REDIRECTION;
    }
    else if(status == ResultView::bad)
    {
        return 
                ls->status() == LinkStatus::BROKEN ||
                ls->status() == LinkStatus::HTTP_CLIENT_ERROR ||
                ls->status() == LinkStatus::HTTP_SERVER_ERROR;
    }
    else if(status == ResultView::malformed)
    {
        return ls->status() == LinkStatus::MALFORMED;
    }
    else if(status == ResultView::undetermined)
    {
        return 
                ls->status() == LinkStatus::UNDETERMINED ||
                ls->status() == LinkStatus::TIMEOUT ||
                ls->status() == LinkStatus::NOT_SUPPORTED;
    }
    else
        return true;
}


// ******************************* ResultViewItem *****************************

ResultViewItem::ResultViewItem(LinkStatus const* linkstatus, int column_index)
        : ls_((LinkStatus*)linkstatus), column_index_(column_index)
{
    Q_ASSERT(ls_);
    Q_ASSERT(column_index_ > 0);
}

ResultViewItem::~ResultViewItem()
{}

void ResultViewItem::setColumnIndex(int i)
{
    Q_ASSERT(i > 0);
    column_index_ = i;
}

int ResultViewItem::columnIndex() const
{
    return column_index_;
}

LinkStatus const* ResultViewItem::linkStatus() const
{
    Q_ASSERT(ls_);
    return ls_;
}

QColor const ResultViewItem::textStatusColor() const
{
    if(linkStatus()->errorOccurred())
    {
        //kDebug(23100) <<  "ERROR: " << linkStatus()->error() << ": " << linkStatus()->absoluteUrl().prettyUrl();
        if(linkStatus()->error() == i18n( "Javascript not supported" ))
            return Qt::lightGray;
        else
            return Qt::red;
    }

    else if(linkStatus()->absoluteUrl().hasRef())
        return Qt::blue;

    else if(!linkStatus()->absoluteUrl().protocol().startsWith("http"))
        return Qt::darkGreen;

    else
    {
        QString status_code(QString::number(linkStatus()->httpHeader().statusCode()));

        if(status_code[0] == '0')
        {
            kWarning(23100) << "status code == 0: ";
            kWarning(23100) << LinkStatusHelper::toString(linkStatus());
            kWarning(23100) << linkStatus()->httpHeader().toString();
        }
        //Q_ASSERT(status_code[0] != '0');

        if(status_code[0] == '5')
            return Qt::darkMagenta;

        else if(status_code[0] == '4')
            return Qt::red;

        else if(status_code[0] == '3')
            return Qt::blue;

        else if(status_code[0] == '2')
            return Qt::darkGreen;

        else
            return Qt::red;
    }
}
