/*  This file is part of the KDE project
    Copyright (C) 2008 Paulo Moura Guedes <moura@kdewedev.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "searchmanageragent.h"

#include <KStandardDirs>
#include <ktoolinvocation.h>
#include <kio/netaccess.h>

#include "klsconfig.h"
#include "automationconfig.h"
#include "engine/searchmanager.h"
#include "utils/utils.h"


class SearchManagerAgent::SearchManagerAgentPrivate
{
public:
    SearchManagerAgentPrivate()
        : busy(false), searchManager(0), brokenLinksOnly(false)
    {}
    
    ~SearchManagerAgentPrivate()
    {
        delete searchManager;
        searchManager = 0;
    }
    
    bool busy;
    SearchManager* searchManager;
    QString optionsFilePath;
    bool brokenLinksOnly;
    KUrl exportResultsPath;
    QString mailRecipient;
};

SearchManagerAgent::SearchManagerAgent(QObject *parent)
    : QObject(parent), d(new SearchManagerAgentPrivate())
{
}

SearchManagerAgent::~SearchManagerAgent()
{
    delete d;
}

void SearchManagerAgent::setOptionsFilePath(QString const& optionsFilePath)
{
    d->optionsFilePath = optionsFilePath;
}

void SearchManagerAgent::reset()
{
    d->busy = false;
    delete d->searchManager;
    d->searchManager = 0;
    d->optionsFilePath = QString();
    d->exportResultsPath = KUrl();
    d->brokenLinksOnly = false;
}

void SearchManagerAgent::check()
{
    if(d->optionsFilePath.isEmpty()) {
        kWarning(23100) << "optionsFilePath.isEmpty()";
        return;
    }
    
    check(d->optionsFilePath);
}

void SearchManagerAgent::check(QString const& optionsFilePath)
{
    d->busy = true;
    d->optionsFilePath = optionsFilePath;
  
    delete d->searchManager;
    d->searchManager = new SearchManager(KLSConfig::maxConnectionsNumber(),
                                        KLSConfig::timeOut(),
                                        this);

    if(!initSearchOptions(d->searchManager)) {
        reset();
        return;
    }
    
    connect(d->searchManager, SIGNAL(signalSearchFinished(SearchManager*)),
            this, SLOT(slotExportSearchFinished(SearchManager*)));

    d->searchManager->startSearch();
}

SearchManager* SearchManagerAgent::searchManager() const
{
    return d->searchManager;
}

bool SearchManagerAgent::initSearchOptions(SearchManager* searchManager)
{
    AutomationConfig::instance(QString());
    delete AutomationConfig::self();
    AutomationConfig::instance(d->optionsFilePath);
    AutomationConfig* config = AutomationConfig::self();

//     KConfig config(d->optionsFilePath, KConfig::SimpleConfig);
//     KConfigGroup group = config.group(QString());

    KUrl url(config->urlToCheck());
    
    if(!url.isValid()) {
        kWarning(23100) << "Invalid options defined in file " << d->optionsFilePath;
        return false;
    }

    KUrl documentRoot(config->documentRoot());
    int depth = config->depth();
    bool checkParentFolders = config->checkParentFolders();
    bool checkExternalLinks = config->checkExternalLinks();
    QString doNotCheckRegularExpressionString = config->regularExpression();
    bool doNotCheckRegularExpression = !doNotCheckRegularExpressionString.isEmpty();
    d->brokenLinksOnly = config->brokenLinksOnly();
    d->exportResultsPath = KUrl(config->resultsFilePath() + "/");
    d->mailRecipient = config->mailRecipient();
    
    if(!d->exportResultsPath.isLocalFile()) {
        kWarning(23100) << "Results file path is not local, aborting" << d->optionsFilePath;
        return false;
    }

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

    return true;
}

void SearchManagerAgent::slotExportSearchFinished(SearchManager* searchManager)
{
    kDebug(23100) << "SearchManagerAgent::slotExportSearchFinished";
    kDebug(23100) << d->exportResultsPath;
    
    if(!d->exportResultsPath.isValid()) {
        kWarning(23100) << "exportResultsPath is not valid";
        reset();
        return;
    }
      
    kDebug(23100) << "Exporting results...";
    
    QString preferedStylesheet = KLSConfig::preferedStylesheet();
    KUrl styleSheetUrl = KStandardDirs::locate("appdata", preferedStylesheet);
//     KUrl styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
    kDebug(23100) << "Style sheet URL: " << styleSheetUrl.url();
    if(!styleSheetUrl.isValid()) {
        kWarning(23100) << "Style sheet not valid!";
        styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
    }
 
    LinkStatusHelper::Status status = d->brokenLinksOnly ?  
            LinkStatusHelper::bad : LinkStatusHelper::none;
    QString html = XSL::transform(searchManager->toXML(status), styleSheetUrl);
    
    // export to file
    if(!KIO::NetAccess::exists(d->exportResultsPath, KIO::NetAccess::SourceSide, 0)) {
        kDebug(23100) << "Creating directory: " << d->exportResultsPath;
        if(!KIO::NetAccess::mkdir(d->exportResultsPath, 0)) {
            kWarning(23100) << "Could not create directory: " << d->exportResultsPath;
            return;
        }
    }
        
    bool passed = searchManager->searchCounters().brokenLinks() == 0 ? true : false;
    QString passedString = passed ? "passed" : "broken";
    QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhh");

    KUrl filePath(d->exportResultsPath.url() + "linkcheck-" + passedString + "-"
        + dateTime + ".html");

    kDebug(23100) << "Exporting results to file: " << filePath.url();

    FileManager::write(html, filePath);

    // E-mail site administrator
    if(!d->mailRecipient.isEmpty()) {

        // TODO
        QString subject;
        QString body;
        QStringList attachUrls;

        KToolInvocation::invokeMailer(
        /*to*/          d->mailRecipient,
        /*cc*/          QString(),
        /*bcc*/         QString(),
        /*subject*/     subject,
        /*body*/        body,
        /*messageFile*/ QString(),
        /*attachURLs*/  QStringList(),
        /*startup_id*/  "" );
    }

    reset();
}


#include "searchmanageragent.moc"
