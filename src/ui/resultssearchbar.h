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

#ifndef RESULTSSEARCHBAR_H
#define RESULTSSEARCHBAR_H

#include <qstring.h>

#include "resultview.h"
#include "../engine/linkfilter.h"

/**
    @author Paulo Moura Guedes <moura@kdewebdev.org>
    Based on Akregator code. Kudos ;)
*/
class ResultsSearchBar : public QWidget
{
    Q_OBJECT
public:
    ResultsSearchBar(QWidget *parent = 0, const char *name = 0);
    ~ResultsSearchBar();

    QString const& text() const;
    int status() const;

    void setDelay(int ms);
    int delay() const;
    
    LinkMatcher currentLinkMatcher() const;

signals:
    /** emitted when the text and status filters were updated. Params are textfilter, statusfilter */
    void signalSearch(LinkMatcher);

public slots:
    void slotClearSearch();
    void slotSetStatus(int status);
    void slotSetText(const QString& text);

private slots:

    void slotSearchStringChanged(const QString& search);
    void slotSearchComboChanged(int index);
    void slotActivateSearch();

private:
    
    ResultView::Status selectedStatus() const;
    
private:

    class ResultsSearchBarPrivate;
    ResultsSearchBarPrivate* d;
};

#endif
