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

#include "isearchmanager.h"

#include <KStandardDirs>

#include "engine/searchmanager.h"
#include "utils/utils.h"
#include "klsconfig.h"
#include "isearchmanageradaptor.h"


ISearchManager::ISearchManager(QObject* parent)
    : QObject(parent)
{
    new ISearchManagerAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/ISearchManager", this);
}

void ISearchManager::checkAndExportToHtml(QString const& optionsFilePath)
{
    SearchManager* searchManager = new SearchManager(KLSConfig::maxConnectionsNumber(),
                                        KLSConfig::timeOut(),
                                        this);
    m_exportSearchManagerList << searchManager;

    initSearchManager(searchManager, optionsFilePath);

    connect(searchManager, SIGNAL(signalSearchFinished(SearchManager*)),
            this, SLOT(slotSearchFinished(SearchManager*)));

    searchManager->startSearch();
}

// void ISearchManager::checkAndMailResuls(QString const& optionsFilePath)
// {
// 
// }

void ISearchManager::initSearchManager(SearchManager* searchManager, QString const& optionsFilePath)
{
    KConfig config(optionsFilePath, KConfig::SimpleConfig);
    KConfigGroup group = config.group(QString());

    KUrl url(group.readEntry("URL"));
    int depth = group.readEntry("Depth", -1);
    bool checkParentFolders = group.readEntry("CheckParentFolders", true);
    bool checkExternalLinks = group.readEntry("CheckExternalLinks", true);
    QString doNotCheckRegularExpressionString = group.readEntry("DoNotCheckRegularExpression", "");
    bool doNotCheckRegularExpression = !doNotCheckRegularExpressionString.isEmpty();
    KUrl resultsFile(group.readEntry("ResultsFile", ""));

    searchManager->setRootUrl(url);
    searchManager->setDepth(depth);
    searchManager->setCheckParentDirs(checkParentFolders);
    searchManager->setCheckExternalLinks(checkExternalLinks);

    if(doNotCheckRegularExpression) {
        searchManager->setCheckRegularExpressions(true);
        searchManager->setRegularExpression(doNotCheckRegularExpressionString, false);
    }
    else {
        searchManager->setCheckRegularExpressions(false);
    }

    m_exportResultsPath = resultsFile;
}

void ISearchManager::slotExportSearchFinished(SearchManager* searchManager)
{
    Q_ASSERT(m_exportResultsPath.isValid());

    KUrl styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
    QString html = XSL::transform(searchManager->toXML(), styleSheetUrl);

    FileManager::write(html, m_exportResultsPath);
}

void ISearchManager::slotEmailSearchFinished(SearchManager* /*searchManager*/)
{

}


#include "isearchmanager.moc"
