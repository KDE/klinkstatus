//
// C++ Implementation: resultlinkview
//
// Description:
//
//
// Author: Paulo Moura Guedes <pmg@netcabo.pt>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "resultview.h"
#include "../engine/linkstatus.h"

#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>


ResultView::ResultView(int column_index_status,
                       int column_index_label,
                       int column_index_URL)
        : col_status_(column_index_status),
        col_label_(column_index_label),
        col_url_(column_index_URL),
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
        columns_.push_back(columns[i]);

        if(columns[i] == "Status")
            Q_ASSERT(i + 1 == col_status_);
        else if(columns[i] == "Label")
            Q_ASSERT(i + 1 == col_label_);
        else if(columns[i] == "URL")
            Q_ASSERT(i + 1 == col_url_);
    }
}

bool ResultView::displayableWithStatus(LinkStatus const* ls, Status const& status)
{
    if(status == ResultView::good)
    {
        if(ls->errorOccurred())
            return false;
        else
            if(ls->absoluteUrl().protocol() != "http" &&
                    ls->absoluteUrl().protocol() != "https")
                return (ls->status() == "OK" or
                        (not ls->absoluteUrl().hasRef()));
            else
            {
                QString status_code(QString::number(ls->httpHeader().statusCode()));
                return (ls->status() == "OK" or
                        (not ls->absoluteUrl().hasRef() and
                         status_code[0] != '5' and
                         status_code[0] != '4'));
            }
    }
    else if(status == ResultView::bad)
    {
        return (not displayableWithStatus(ls, ResultView::good) and not ls->error().contains("Timeout"));
    }
    else if(status == ResultView::malformed)
    {
        return (ls->error() == "Malformed");
    }
    else if(status == ResultView::undetermined)
    {
        return (ls->error().contains("Timeout") or
                (ls->absoluteUrl().hasRef() and ls->status() != "OK"));
    }
    else
        return true;
}

