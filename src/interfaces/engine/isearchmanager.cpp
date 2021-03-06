/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
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

#include "isearchmanager.h"

#include "isearchmanageradaptor.h"
#include "engine/searchmanager.h"


ISearchManager::ISearchManager(SearchManager* searchManager)
    : QDBusAbstractAdaptor(searchManager), m_searchManager(searchManager)
{
    new ISearchManagerAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
//     dbus.registerObject("/SearchManager", this);

    setAutoRelaySignals(true);
}


ISearchManager::~ISearchManager()
{
}

int ISearchManager::numberOfCheckedLinks() const
{
    return m_searchManager->checkedLinks();
}

int ISearchManager::numberOfGoodLinks() const
{
    return m_searchManager->searchCounters().totalLinks();
}

int ISearchManager::numberOfBrokenLinks() const
{
    return m_searchManager->searchCounters().brokenLinks();
}

int ISearchManager::numberOfUndeterminedLinks() const
{
    return m_searchManager->searchCounters().undeterminedLinks();
}


#include "isearchmanager.moc"
