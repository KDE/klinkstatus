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
#include "tabwidgetsession.h"
#include "sessionwidget.h"
#include "../engine/searchmanager.h"

#include <qtoolbutton.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qiconset.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <klocale.h>


TabWidgetSession::TabWidgetSession(QWidget* parent, const char* name, WFlags f)
        : QTabWidget(parent, name, f) // tabs_ is initialized with size 17
{
    setFocusPolicy( QTabWidget::NoFocus );
    setMargin( 0 );

    tabs_.setAutoDelete(true);
}

TabWidgetSession::~TabWidgetSession()
{}

SessionWidget* TabWidgetSession::currentSession() const
{
    return tabs_[currentPageIndex()];
}

bool TabWidgetSession::emptySessionsExist() const
{
    if(count() == 0)
        return true;

    for(int i = 0; i != count(); ++i)
    {
		assert(tabs_[i]);
        if(tabs_[i]->isEmpty() and not tabs_[i]->getSearchManager()->searching())
            return true;
    }
    return false;
}

SessionWidget* TabWidgetSession::getEmptySession() const
{
    assert(emptySessionsExist());
    assert(count() != 0);

    for(uint i = 0; i != tabs_.size(); ++i)
    {
        if(tabs_[i]->isEmpty())
            return tabs_[i];
    }
	return 0;
}

void TabWidgetSession::newSession()
{
    // TODO: settings: number of connections, timeout
    SessionWidget* session_widget = newSessionWidget();
    connect(session_widget, SIGNAL(signalCheckUrl(const LinkStatus * )),
            this, SLOT(updateTabLabel(const LinkStatus * )));

    insertTab(session_widget, QString("Session") + QString::number(count() + 1));
    tabs_.insert(count() - 1, session_widget);
	assert(tabs_[count() - 1]);
    setCurrentPage(count() - 1);
}

void TabWidgetSession::newSession(KURL const& url)
{
    newSession();
    currentSession()->setUrl(url);
}

void TabWidgetSession::closeSession()
{
    if(count() > 1)
        removePage(currentPage());
}

SessionWidget* TabWidgetSession::newSessionWidget()
{
    // TODO: settings: number of connections, timeout
    SessionWidget* session_widget = new SessionWidget( 5, 30, this, "session_widget");

    vector<QString> columns;
    columns.push_back(i18n("Status"));
    columns.push_back(i18n("Label"));
    columns.push_back(i18n("URL"));
    session_widget->setColumns(columns);

    return session_widget;
}

void TabWidgetSession::updateTabLabel(LinkStatus const* linkstatus)
{
    KURL url = linkstatus->absoluteUrl();
    changeTab(currentPage(), url.prettyURL());
}

void TabWidgetSession::setUrl(KURL const& url)
{
    currentSession()->setUrl(url);
}

#include "tabwidgetsession.moc"
