/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   kde@mouraguedes.com                                                        *
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

#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <kurl.h>
#include <threadweaver/Job.h>
#include <threadweaver/ThreadWeaver.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QRegExp>
#include <QHash>
#include <QMutex>
class QDomElement;

#include <QList>

#include "klinkstatus_export.h"
#include "engine/linkstatus.h"
#include "engine/linkstatushelper.h"
#include "engine/linkchecker.h"
#include "parser/node.h"
#include "parser/url.h"
#include "parser/robotsparser.h"
class AddLevelJob;

using namespace ThreadWeaver;


class SearchCounters
{
public:
    friend class SearchManager;
    
    SearchCounters();
    
    void updateCounters(LinkStatus* link);
    void resetCounters(LinkStatus* link);

    int totalLinks() const;
    int brokenLinks() const;
    int undeterminedLinks() const;
    
private:
    int total_links_;
    int broken_links_;
    int undetermined_links_;
};


typedef QHash<QString, KHTMLPart*> KHTMLPartMap;

class KLINKSTATUS_EXPORT SearchManager: public QObject
{
    Q_OBJECT

public:

    friend class BuildNodeJob;
    friend class AddLevelJob;
    friend class ISearchManager;

    enum SearchMode {
        depth,
        domain,
        depth_and_domain
    };

    explicit SearchManager(int max_simultaneous_connections = 5, int time_out = 50,
                  QObject *parent = 0);
    ~SearchManager();

    /** Returns the results in XML format */
    QString toXML(LinkStatusHelper::Status status = LinkStatusHelper::none) const;
    QString buildSiteMapXml() const;
    
    void startSearch();
    void startSearch(KUrl const& root);
    void startSearch(KUrl const& root, SearchMode const& mode);
    void resume();
    void cancelSearch();

    int checkedLinks() const;
    SearchCounters const& searchCounters() const;

    void recheckLink(LinkStatus* linkStatus);
    void recheckLinks(QList<LinkStatus*> const& linkstatus_list);

    /** Checks if each document in the list is referred at least once in the search results */
    QStringList findUnreferredDocuments(KUrl const& baseDir, QStringList const& documentList) const;

    QList<LinkStatus*> getLinksWithHtmlProblems() const;

    /** Document root is used when protocol is different that HTTP, in order to resolve absolute URLs */
    bool hasDocumentRoot() const;
    KUrl const& documentRoot() const;
    void setDocumentRoot(KUrl const& url);

    QString searchProtocol() const { return root_.absoluteUrl().protocol(); };

    void setRootUrl(KUrl const& url);
    void setIsLoginPostRequest(bool is);
    void setPostUrl(QString const& url);
    void setPostData(QByteArray const& data);
    void setSearchMode(SearchMode modo);
    void setDepth(int depth);
    void setDomain(QString const& domain);
    void setCheckParentDirs(bool flag);
    void setCheckExternalLinks(bool flag);
    void setCheckRegularExpressions(bool flag);
    void setRegularExpression(QString const& reg_exp, bool case_sensitive);
    void setTimeOut(int time_out);

    bool searching() const;
    bool localDomain(KUrl const& url, bool restrict = true) const;
    bool existUrl(KUrl const& url, KUrl const& url_parent) const;
    LinkStatus const* linkStatus(QString const& s_url) const;
    LinkStatus const* linkStatusRoot() const;

    bool sendIdentification() const { return send_identification_; }
    QString const& userAgent() const { return user_agent_; }

    KHTMLPart* htmlPart(QString const& key_url) const;
    void addHtmlPart(QString const& key_url, KHTMLPart* html_part);

    ThreadWeaver::Weaver* threadWeaver() const;
    
signals:

    void signalRootChecked(LinkStatus* link);
    void signalLinkChecked(LinkStatus* link);
    void signalLinkRechecked(LinkStatus* link);
    void signalSearchFinished(SearchManager*);
    void signalSearchPaused();
    void signalNewLinksToCheck(int number_of_links);
    void signalAddingLevel(bool adding);
    void signalLinksToCheckTotalSteps(int links_to_check);
    void signalRedirection();
    void signalUnreferredDocStepCompleted() const;
    void signalUnreferredDocFound(const QString&) const;

private slots:

    void slotRootChecked(LinkStatus* link, LinkChecker* checker);
    void slotLinkChecked(LinkStatus* link, LinkChecker* checker);
    void slotLinkRechecked(LinkStatus* link, LinkChecker* checker);
    void slotLevelAdded();
    void slotJobDone(ThreadWeaver::Job*);
    void finnish();

private:

    void save(QDomElement& element, LinkStatusHelper::Status status = LinkStatusHelper::none) const;
    void cleanItems();
    void reset();
    SearchMode const& searchMode() const;
    void initRobotsParser(KUrl const& url);
    bool checkRegularExpressions() const { return check_regular_expressions_; }
    bool checkParentDirs() const;
    bool checkExternalLinks() const;
    int maxSimultaneousConnections() const;
    int timeOut() const;
    void removeHtmlParts();
    void checkRoot();
    // Used to search a node of a level, but not necesseraly
    void checkVectorLinks(QList<LinkStatus*> const& links);
    void checkVectorLinksToRecheck(QList<LinkStatus*> const& links);
    void fillWithChildren(LinkStatus* link, QList<LinkStatus*>& children);
    void startSearchAfterRoot();
    void continueSearch();
    void continueRecheck();
    void pause();
    QList<LinkStatus*> const& nodeToAnalize() const;
    void checkLink(LinkStatus* ls, bool recheck = false);
    QList<LinkStatus*> chooseLinks(QList<LinkStatus*> const& links);
    QList<LinkStatus*> chooseLinksToRecheck(QList<LinkStatus*> const& links);
    void checkLinksSimultaneously(QList<LinkStatus*> const& links, bool recheck);
    void buildNewNode(LinkStatus* linkstatus);
    void addLevel();
    bool checkableByDomain(KUrl const& url, LinkStatus const& link_parent) const;
    bool checkable(KUrl const& url, LinkStatus const& link_parent) const;
    int maximumCurrentConnections() const;
    bool onlyCheckHeader(LinkStatus* ls) const;
    void linkRedirectionChecked(LinkStatus* link, bool recheck = false);
    void setExternalDomainDepth(int depth);

    /*
      Entende-se por domain vago um domain do tipo www.google.pt ou google.pt, pelo que,
      por exemplo, imagens.google.pt, e considerado estar no mesmo domain.
      pwp.netcabo.pt ou www.google.pt/imagens nao sao considerados domains vagos.
    */
    bool generalDomain() const;
    bool generalDomainChecked() const; // Para garantir que o procedimento generalDomain() so e chamado uma vez

private:

    SearchCounters search_counters_;

    // authentication
    bool is_login_post_request_;
    QString post_url_;
    QByteArray post_data_;
    
    // whether it is a new search or refreshing previously checked links
    bool recheck_mode_;
    int max_simultaneous_connections_;
    SearchMode search_mode_;
    KUrl root_url_;
    RobotsParser robots_parser_;
    LinkStatus root_;
    bool has_document_root_;
    // in case of non http protocols the document root must be explicitly given
    KUrl document_root_url_; 
    int depth_;
    int current_depth_;
    int external_domain_depth_;
    int current_node_;
    int current_index_;
    int links_being_checked_;
    int finished_connections_;
    int maximum_current_connections_;
    QRegExp reg_exp_;
    QString domain_;
    bool general_domain_;
    bool checked_general_domain_;
    int time_out_;
    int current_connections_;
    bool send_identification_; // user-agent
    QString user_agent_;
    
    bool canceled_;
    bool searching_;
    int ignored_links_;
    bool check_parent_dirs_;
    bool check_external_links_;
    bool check_regular_expressions_;
    int number_of_current_level_links_;
    QList< QList< QList <LinkStatus*> > > search_results_;
    QHash<KUrl, LinkStatus*> search_results_hash_;
    QList<QList<LinkStatus*> > new_level_;
        
    QList<LinkStatus*> recheck_links_;
    int links_rechecked_;
    int recheck_current_index_;

    KHTMLPartMap html_parts_;

    // thread stuff
    mutable Weaver m_weaver;
    mutable QMutex m_mutex;
};

class BuildNodeJob : public Job
{
    Q_OBJECT
public:
    BuildNodeJob(SearchManager* manager, LinkStatus* linkstatus);
    ~BuildNodeJob();
    
protected:
    void run();

private:
    SearchManager* m_searchManager;
    LinkStatus* m_linkStatus;
};

class AddLevelJob : public Job
{
    Q_OBJECT
public:
    AddLevelJob(SearchManager* manager);
    ~AddLevelJob();
    
protected:
    void run();

private:
    SearchManager* m_searchManager;
};

#include "searchmanager_impl.h"

#endif // SEARCHMANAGER_H
