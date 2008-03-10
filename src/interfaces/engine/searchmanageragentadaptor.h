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

#ifndef ISEARCH_MANAGER
#define ISEARCH_MANAGER

#include <QObject>
#include <QtDBus/QtDBus>

#include <KUrl>

#include <QSet>

#include "klinkstatus_export.h"
#include "engine/searchmanager.h"
#include "searchmanageradaptor.h"
class SearchManager;
class SearchManagerAgent;


class KLINKSTATUS_EXPORT SearchManagerAgentAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdewebdev.klinkstatus.SearchManagerAgent")

public:
    explicit SearchManagerAgentAdaptor(SearchManagerAgent* searchManagerAgent);

    SearchManagerAdaptor* searchManager();

public Q_SLOTS:
    void check(QString const& optionsFilePath);

private:
    SearchManagerAgent* m_searchManagerAgent;
    SearchManagerAdaptor* m_searchManagerAdaptor;
};

#endif
