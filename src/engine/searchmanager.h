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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GESTOR_PESQUISA_H
#define GESTOR_PESQUISA_H

#include <vector>

#include <qobject.h>
#include <qstring.h>
#include <kurl.h>
#include <qdatetime.h>

#include "linkstatus.h"
#include "linkchecker.h"
#include "../parser/node.h"
#include "../parser/url.h"

using namespace std;


class SearchManager: public QObject
{
    Q_OBJECT

public:

    enum SearchMode {
        depth,
        domain,
        depth_and_domain
    };

    SearchManager(int max_simultaneous_connections = 3, int time_out = 50,
                  QObject *parent = 0, const char *name = 0);
    ~SearchManager();

    void startSearch(KURL const& root);
    void startSearch(KURL const& root, SearchMode const& modo);
    void resume();
    void cancelSearch();

    void setSearchMode(SearchMode modo);
    void setDepth(int depth);
    void setExternalDomainDepth(int depth);
    void setDomain(QString const& domain);
    void setCheckParentDirs(bool flag);
	void setCheckExternalLinks(bool flag);
    void setTimeOut(int time_out);
	
    void cleanItems();
    void reset();

    bool searching() const;
    bool localDomain(KURL const& url, bool restrict = true) const;
    //bool isLocalRestrict(KURL const& url) const;
    SearchMode const& searchMode() const;
    bool existUrl(KURL const& url, KURL const& url_parent) const;
    LinkStatus const* linkStatus(QString const& s_url) const;
    int checkedLinks() const;
    QTime timeElapsed() const;
    bool checkParentDirs() const;
	bool checkExternalLinks() const;
    LinkStatus const* linkStatusRoot() const;
    int maxSimultaneousConnections() const;
    int timeOut() const;

private:

    void checkRoot();
    void checkVectorLinks(vector<LinkStatus*> const& links); // corresponde a um no de um nivel de depth
    vector<LinkStatus*> children(LinkStatus* link);
    void startSearch();
    void continueSearch();
    void finnish();
    void pause();
    vector<LinkStatus*> const& nodeToAnalize() const;
    vector<LinkStatus*> chooseLinks(vector<LinkStatus*> const& links);
    void checkLinksSimultaneously(vector<LinkStatus*> const& links);
    void addLevel();
    bool checkableByDomain(KURL const& url, LinkStatus const& link_parent) const;
    bool checkable(KURL const& url, LinkStatus const& link_parent) const;
    int maximumCurrentConnections() const;
    bool onlyCheckHeader(LinkStatus* ls) const;

    /*
      Entende-se por domain vago um domain do tipo www.google.pt ou google.pt, pelo que,
      por exemplo, imagens.google.pt, e considerado estar no mesmo domain.
      pwp.netcabo.pt ou www.google.pt/imagens nao sao considerados domains vagos.
    */
    bool generalDomain() const;
    bool generalDomainChecked() const; // Para garantir que o procedimento generalDomain() so e chamado uma vez

private slots:

    void slotRootChecked(const LinkStatus * link, LinkChecker * checker);
    void slotLinkChecked(const LinkStatus * link, LinkChecker * checker);
    void slotSearchFinished();
    void slotLinkCheckerFinnished(LinkChecker * checker);

signals:

    void signalRootChecked(const LinkStatus * link, LinkChecker * checker);
    void signalLinkChecked(const LinkStatus * link, LinkChecker * checker);
    void signalSearchFinished();
    void signalSearchPaused();
    void signalAddingLevelTotalSteps(uint number_of_links);
    void signalAddingLevelProgress();
    void signalLinksToCheckTotalSteps(uint links_to_check);
    //void signalLinksToCheckProgress();

private:

    int max_simultaneous_connections_;
    SearchMode search_mode_;
    LinkStatus root_;
    int depth_;
    int current_depth_;
    int external_domain_depth_;
    int current_node_;
    int current_index_;
    int links_being_checked_;
    int finished_connections_;
    int maximum_current_connections_;
    QString domain_;
    bool general_domain_;
    bool checked_general_domain_;
    //QMutex mutex;
    int time_out_;
    int current_connections_;
    bool canceled_;
    bool searching_;
    int checked_links_;
    QTime time_;
    int ignored_links_;
    bool check_parent_dirs_;
	bool check_external_links_;
    uint number_of_level_links_;
    uint number_of_links_to_check_;
    vector< vector< vector <LinkStatus*> > > search_results_;
};

#include "searchmanager_impl.h"

#endif
