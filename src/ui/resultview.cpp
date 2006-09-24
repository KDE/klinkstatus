//
// C++ Implementation: resultlinkview
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "resultview.h"
#include "../engine/linkstatus.h"

#include <qpainter.h>
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
    for(uint i = 0; i != columns.size(); ++i)
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

QColor const& ResultViewItem::textStatusColor() const
{
    if(linkStatus()->errorOccurred())
    {
        //kdDebug(23100) <<  "ERROR: " << linkStatus()->error() << ": " << linkStatus()->absoluteUrl().prettyURL() << endl;
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
            kdWarning(23100) <<  "status code == 0: " << endl;
            kdWarning(23100) <<  linkStatus()->toString() << endl;
            kdWarning(23100) <<  linkStatus()->httpHeader().toString() << endl;
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








