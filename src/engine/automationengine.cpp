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

#include "automationengine.h"

#include <KDebug>

#include <QDir>
#include <QStringList>

#include "engine/searchmanageragent.h"
#include "engine/searchmanager.h"


void AutomationEngine::startLinkCheck()
{
    kDebug(23100) << "AutomationEngine::initLinkChecks";

    QDir dir(m_configurationFilesDir);
    dir.setNameFilters(QStringList("*.properties"));

    QStringList configurationFiles = dir.entryList();
    //kDebug(23100) << configurationFiles;

    if(configurationFiles.empty()) {
        kDebug(23100) << "no configuration files available";
        emit signalSearchFinished();
        return;
    }
    
    foreach(const QString &file, configurationFiles) {
        check(file);
    }
}

void AutomationEngine::check(QString const& configurationFile)
{
    kDebug(23100) << "configurationFile: " << configurationFile;
    
    SearchManagerAgent* agent = new SearchManagerAgent(this);
    agent->setOptionsFilePath(m_configurationFilesDir + "/" + configurationFile);

    connect(agent, SIGNAL(signalSearchFinished(SearchManager*)),
        this, SIGNAL(signalSearchFinished()));

    agent->check();
}


#include "automationengine.moc"
