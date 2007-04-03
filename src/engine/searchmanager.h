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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef GESTOR_PESQUISA_H
#define GESTOR_PESQUISA_H

#include <kurl.h>
#include <threadweaver/Job.h>

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QRegExp>
#include <QHash>
class QDomElement;

#include <vector>

#include "engine/linkstatus.h"
#include "engine/linkchecker.h"
#include "parser/node.h"
#include "parser/url.h"
class AddLevelJob;
         
using namespace std;
using namespace ThreadWeaver;


typedef QHash<QString, KHTMLPart*> KHTMLPartMap;

class SearchManager: public QObject
{
    Q_OBJECT

public:

    friend class AddLevelJob;

    enum SearchMode {
        depth,
        domain,
        depth_and_domain
    };

    SearchManager(int max_simultaneous_connections = 3, int time_out = 50,
                  QObject *parent = 0);
    ~SearchManager();

    /** Returns the the results in XML format */
    QString toXML() const;

    void startSearch(KUrl const& root);
    void startSearch(KUrl const& root, SearchMode const& mode);
    void resume();
    void cancelSearch();

    void recheckLink(KUrl const& url);

    bool hasDocumentRoot() const;
    KUrl const& documentRoot() const;
    void setDocumentRoot(KUrl const& url);

    QString searchProtocol() const { return root_.absoluteUrl().protocol(); };

    void setSearchMode(SearchMode modo);
    void setDepth(int depth);
    void setExternalDomainDepth(int depth);
    void setDomain(QString const& domain);
    void setCheckParentDirs(bool flag);
    void setCheckExternalLinks(bool flag);
    void setCheckRegularExpressions(bool flag);
    void setRegularExpression(QString const& reg_exp, bool case_sensitive);
    void setTimeOut(int time_out);

    bool searching() const;
    bool localDomain(KUrl const& url, bool restrict = true) const;
    //bool isLocalRestrict(KUrl const& url) const;
    bool existUrl(KUrl const& url, KUrl const& url_parent) const;
    LinkStatus const* linkStatus(QString const& s_url) const;
    LinkStatus const* linkStatusRoot() const;

    bool sendIdentification() const { return send_identification_; }
    QString const& userAgent() const { return user_agent_; }

    KHTMLPart* htmlPart(QString const& key_url) const;
    void addHtmlPart(QString const& key_url, KHTMLPart* html_part);


signals:

    void signalRootChecked(LinkStatus const* link);
    void signalLinkChecked(LinkStatus const* link);
    void signalLinkRechecked(LinkStatus const* link);
    void signalSearchFinished();
    void signalSearchPaused();
    void signalAddingLevelTotalSteps(uint number_of_links);
    void signalAddingLevelProgress();
    void signalLinksToCheckTotalSteps(uint links_to_check);

private slots:

    void slotRootChecked(LinkStatus* link, LinkChecker* checker);
    void slotLinkChecked(LinkStatus* link, LinkChecker* checker);
    void slotLinkRechecked(LinkStatus* link, LinkChecker* checker);
    void slotSearchFinished();
    void slotLevelAdded();
    void slotJobDone(Job*);

private:

    void save(QDomElement& element) const;
    void cleanItems();
    void reset();
    SearchMode const& searchMode() const;
    bool checkRegularExpressions() const { return check_regular_expressions_; }
    int checkedLinks() const;
    bool checkParentDirs() const;
    bool checkExternalLinks() const;
    int maxSimultaneousConnections() const;
    int timeOut() const;
    void removeHtmlParts();
    void checkRoot();
    void checkVectorLinks(vector<LinkStatus*> const& links); // corresponde a um no de um nivel de depth
    void fillWithChildren(LinkStatus* link, vector<LinkStatus*>& children);
    void startSearch();
    void continueSearch();
    void finnish();
    void pause();
    vector<LinkStatus*> const& nodeToAnalize() const;
    vector<LinkStatus*> chooseLinks(vector<LinkStatus*> const& links);
    void checkLinksSimultaneously(vector<LinkStatus*> const& links);
    void addLevel();
    bool checkableByDomain(KUrl const& url, LinkStatus const& link_parent) const;
    bool checkable(KUrl const& url, LinkStatus const& link_parent) const;
    int maximumCurrentConnections() const;
    bool onlyCheckHeader(LinkStatus* ls) const;
    void linkRedirectionChecked(LinkStatus* link);

    /*
      Entende-se por domain vago um domain do tipo www.google.pt ou google.pt, pelo que,
      por exemplo, imagens.google.pt, e considerado estar no mesmo domain.
      pwp.netcabo.pt ou www.google.pt/imagens nao sao considerados domains vagos.
    */
    bool generalDomain() const;
    bool generalDomainChecked() const; // Para garantir que o procedimento generalDomain() so e chamado uma vez

private:

    int max_simultaneous_connections_;
    SearchMode search_mode_;
    LinkStatus root_;
    bool has_document_root_;
    KUrl document_root_url_; // in case of non http protocols the document root must be explicitly given
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
    int checked_links_;
    int ignored_links_;
    bool check_parent_dirs_;
    bool check_external_links_;
    bool check_regular_expressions_;
    uint number_of_current_level_links_;
    uint number_of_new_links_to_check_;
    vector< vector< vector <LinkStatus*> > > search_results_;
    QHash<KUrl, LinkStatus*> search_results_hash_;
    KHTMLPartMap html_parts_;

    // thread stuff
    AddLevelJob* m_addLevelJob;
};

class AddLevelJob : public Job
{
    Q_OBJECT
public:
    AddLevelJob(SearchManager& manager);
    ~AddLevelJob();
    
protected:
    void run();

private:
    SearchManager& m_searchManager;
};


#include "searchmanager_impl.h"

#endif
