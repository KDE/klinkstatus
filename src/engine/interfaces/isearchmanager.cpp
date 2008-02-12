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
#include <kio/netaccess.h>

#include "engine/searchmanager.h"
#include "utils/utils.h"
#include "klsconfig.h"
#include "isearchmanageradaptor.h"


ISearchManager::ISearchManager(QObject* parent)
    : QObject(parent), m_isValidInput(false), m_brokenLinksOnly(true)
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
    if(!m_isValidInput) {
        return;
    }
    
    connect(searchManager, SIGNAL(signalSearchFinished(SearchManager*)),
            this, SLOT(slotExportSearchFinished(SearchManager*)));

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
    
    m_isValidInput = true;
    if(!url.isValid()) {
        kWarning(23100) << "Invalid options defined in file " << optionsFilePath;
        m_isValidInput = false;
        return;
    }

    KUrl documentRoot(group.readEntry("DocumentRoot"));
    int depth = group.readEntry("Depth", -1);
    bool checkParentFolders = group.readEntry("CheckParentFolders", true);
    bool checkExternalLinks = group.readEntry("CheckExternalLinks", true);
    QString doNotCheckRegularExpressionString = group.readEntry("DoNotCheckRegularExpression", "");
    bool doNotCheckRegularExpression = !doNotCheckRegularExpressionString.isEmpty();
    KUrl resultsFileDir(group.readEntry("ResultsFilePath", "") + "/");

    if(!resultsFileDir.isLocalFile()) {
        kWarning(23100) << "Results file path is not local, aborting" << optionsFilePath;
        m_isValidInput = false;
        return;
    }

    m_brokenLinksOnly = group.readEntry("BrokenLinksOnly", true);

    searchManager->setRootUrl(url);
    if (documentRoot.isValid()) {
        searchManager->setDocumentRoot(documentRoot);
    }

    searchManager->setDepth(depth);

    SearchManager::SearchMode mode = depth == -1 ? SearchManager::domain
        : SearchManager::depth_and_domain;
    searchManager->setSearchMode(mode);

    searchManager->setCheckParentDirs(checkParentFolders);
    searchManager->setCheckExternalLinks(checkExternalLinks);

    if(doNotCheckRegularExpression) {
        searchManager->setCheckRegularExpressions(true);
        searchManager->setRegularExpression(doNotCheckRegularExpressionString, false);
    }
    else {
        searchManager->setCheckRegularExpressions(false);
    }

    if(!KIO::NetAccess::exists(resultsFileDir, KIO::NetAccess::SourceSide, 0)) {
        kDebug(23100) << "Creating directory: " << resultsFileDir;
        KIO::NetAccess::mkdir(resultsFileDir, 0);
    }

    m_exportResultsPath = resultsFileDir;
}

void ISearchManager::slotExportSearchFinished(SearchManager* searchManager)
{
    Q_ASSERT(m_exportResultsPath.isValid());

    KUrl styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
 
    LinkStatusHelper::Status status = m_brokenLinksOnly ?  LinkStatusHelper::bad : LinkStatusHelper::none;
    QString html = XSL::transform(searchManager->toXML(status), styleSheetUrl);

    bool passed = searchManager->searchCounters().brokenLinks() == 0 ? true : false;
    QString passedString = passed ? "passed" : "broken";
    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhh");

    KUrl filePath(m_exportResultsPath.url() + "linkcheck-" + passedString + "-" + dateTime + ".html");
    FileManager::write(html, filePath);
}

void ISearchManager::slotEmailSearchFinished(SearchManager* /*searchManager*/)
{

}


#include "isearchmanager.moc"
