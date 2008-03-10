/***************************************************************************
 *   Copyright (C) 2007 by Paulo Moura Guedes                              *
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

#include "searchmanageragentadaptor.h"

#include <KStandardDirs>
#include <kio/netaccess.h>
#include <kross/core/action.h>

#include "engine/searchmanager.h"
#include "engine/searchmanageragent.h"
#include "utils/utils.h"
#include "klsconfig.h"
#include "isearchmanageragentadaptor.h"


SearchManagerAgentAdaptor::SearchManagerAgentAdaptor(SearchManagerAgent* parent)
    : QDBusAbstractAdaptor(parent), m_searchManagerAgent(parent)
{
    new ISearchManagerAgentAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/SearchManagerAgent", this);
}

void SearchManagerAgentAdaptor::check(QString const& optionsFilePath)
{
    m_searchManagerAgent->check(optionsFilePath);
    m_searchManagerAdaptor = new SearchManagerAdaptor(m_searchManagerAgent->searchManager());
}

SearchManagerAdaptor* SearchManagerAgentAdaptor::searchManager()
{
    if(!m_searchManagerAgent->searchManager()) {
        return 0;
    }
    
    if(!m_searchManagerAdaptor) {
        m_searchManagerAdaptor = new SearchManagerAdaptor(m_searchManagerAgent->searchManager());
    }
    
    return m_searchManagerAdaptor;
}


#include "searchmanageragentadaptor.moc"
