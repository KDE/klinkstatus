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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TABWIDGETSESSION_H
#define TABWIDGETSESSION_H

#include <qtabwidget.h>
#include <qintdict.h>

class SessionWidget;
class LinkStatus;

class KURL;


/**
This class handles the creation and destruction of sessions, i.e, severals instances of searching tabs.
 
@author Paulo Moura Guedes
*/
class TabWidgetSession : public QTabWidget
{
    Q_OBJECT

public:
    TabWidgetSession(QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    ~TabWidgetSession();

    /** Set the URL in the current session widget */
    void setUrl(KURL const& url);

    SessionWidget* currentSession() const;
    bool emptySessionsExist() const;
    /** Returns the first empty session it finds */
    SessionWidget* getEmptySession() const;

public slots:
    SessionWidget* newSession();
    SessionWidget* newSession(KURL const& url);
    void closeSession();
    void updateTabLabel(LinkStatus const* linkstatus);

private:
    SessionWidget* newSessionWidget();

private:
    QIntDict<SessionWidget> tabs_;
};

#endif
