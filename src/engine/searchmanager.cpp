/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   pmg@netcabo.pt                                                        *
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

#include "searchmanager.h"
#include "../parser/mstring.h"

#include <qstring.h>

#include <kapplication.h>
#include <kdebug.h>

#include <cassert>
#include <iostream>
#include <unistd.h>

SearchManager::SearchManager(int max_simultaneous_connections, int time_out,
                             QObject *parent, const char *name)
        : QObject(parent, name),
        max_simultaneous_connections_(max_simultaneous_connections),
        depth_(-1), current_depth_(0), external_domain_depth_(0),
        current_node_(0), current_index_(0), links_being_checked_(0),
        finished_connections_(max_simultaneous_connections_),
        maximum_current_connections_(-1), general_domain_(false),
        checked_general_domain_(false), time_out_(time_out), current_connections_(0),
        canceled_(false), searching_(false), checked_links_(0), ignored_links_(0),
        check_parent_dirs_(true), check_external_links_(true),
        number_of_level_links_(0), number_of_links_to_check_(0)
{
    root_.setIsRoot(true);
}

void SearchManager::reset()
{
    kdDebug(23100) <<  "SearchManager::reset()" << endl;

    //assert(not links_being_checked_);

    root_.reset();
    cleanItems();
    depth_ = -1;
    current_depth_ = 0;
    current_node_ = 0;
    current_index_ = 0;
    finished_connections_ = max_simultaneous_connections_;
    domain_ = "";
    maximum_current_connections_ = -1;
    general_domain_ = false;
    checked_general_domain_ = false;
    current_connections_ = 0;
    canceled_ = false;
    searching_ = false;
    checked_links_ = 0;
}

SearchManager::~SearchManager()
{
    reset();
}

void SearchManager::cleanItems()
{
    for(uint i = 0; i != search_results_.size(); ++i)
    {
        for(uint j = 0; j != search_results_[i].size() ; ++j)
        {
            for(uint l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                if(((search_results_[i])[j])[l] != 0)
                {
                    delete ((search_results_[i])[j])[l];
                    ((search_results_[i])[j])[l] = 0;
                }
                else
                    kdDebug(23100) <<  "LinkStatus NULL!!" << endl;
            }
            search_results_[i][j].clear();
        }
        search_results_[i].clear();
    }
    search_results_.clear();
    kdDebug(23100) <<  endl;
}

void SearchManager::startSearch(KURL const& root, SearchMode const& modo)
{
    canceled_ = false;

    //time_.restart();
    time_.start();

    assert(root.isValid());
    //assert(root.protocol() == "http" || root.protocol() == "https");

    if(root.hasHost() && (domain_.isNull() || domain_.isEmpty()))
    {
        setDomain(root.host() + root.directory());
        kdDebug(23100) << "Domain: " << domain_ << endl;
    }
    root_.setIsRoot(true);
    root_.setDepth(0);
    root_.setOriginalUrl(root.prettyURL());
    root_.setAbsoluteUrl(root);
    root_.setOnlyCheckHeader(false);
    root_.setRootUrl(root);

    search_mode_ = modo;
    if(modo == depth)
        assert(depth_ != -1);
    else if(modo == domain)
        assert(depth_ == -1);
    else
        assert(depth_ != -1);

    searching_ = true;

    //assert(domain_ != QString::null);
    checkRoot();
}

void SearchManager::finnish()
{
    searching_ = false;
    while(links_being_checked_)
    {
        kdDebug(23100) <<  "links_being_checked_: " << links_being_checked_ << endl;
        sleep(1);
    }
    emit signalSearchFinished();
}

void SearchManager::cancelSearch()
{
    canceled_ = true;
}

void SearchManager::checkRoot()
{
    LinkChecker* checker = new LinkChecker(&root_, time_out_, this, "link_checker");
    connect(checker, SIGNAL(transactionFinished(const LinkStatus *, LinkChecker *)),
            this, SLOT(slotRootChecked(const LinkStatus *, LinkChecker *)));
    /*
    	connect(checker, SIGNAL(jobFinnished(LinkChecker *)),
                this, SLOT(slotLinkCheckerFinnished(LinkChecker *)));
    */
    checker->check();
}

void SearchManager::slotRootChecked(const LinkStatus * link, LinkChecker * checker)
{
    kdDebug(23100) <<  "SearchManager::slotRootChecked" << endl;

    assert(checked_links_ == 0);
    assert(search_results_.size() == 0);

    ++checked_links_;
    //kdDebug(23100) <<  "++checked_links_: SearchManager::slotRootChecked" << endl;
    emit signalRootChecked(link, checker);

    if(search_mode_ != depth || depth_ > 0)
    {
        current_depth_ = 1;

        vector<LinkStatus*> no = children(LinkStatus::lastRedirection(&root_));

        emit signalLinksToCheckTotalSteps(no.size());

        vector< vector<LinkStatus*> > nivel;
        nivel.push_back(no);

        search_results_.push_back(nivel);

        if(search_results_.size() != 1)
        {
            kdDebug(23100) <<  "search_results_.size() != 1:" << endl;
            kdDebug(23100) <<  "size: " << search_results_.size() << endl;
        }
        assert(search_results_.size() == 1);

        if(no.size() > 0)
        {
            startSearch();
        }
        else
        {
            kdDebug(23100) <<  "Search Finished! (SearchManager::slotRootChecked)#1" << endl;
            finnish();
        }
    }

    else
    {
        assert(search_results_.size() == 0);
        kdDebug(23100) <<  "Search Finished! (SearchManager::slotRootChecked)#2" << endl;
        finnish();
    }

    delete checker;
    checker = 0;
}

vector<LinkStatus*> SearchManager::children(LinkStatus* link)
{
    vector<LinkStatus*> children;
    vector<Node*> const& nodes = link->childrenNodes();

    int count = 0;
    for(uint i = 0; i != nodes.size(); ++i)
    {
        ++count;

        Node* node = nodes[i];
        KURL url;
        if(node->url().isEmpty())
            url = "";
        else
            url = ::normalizeUrl( node->url(), *link );

        if( (node->isLink() &&
                checkable(url, *link) &&
                !::existUrl(url, children) &&
                !node->url().isEmpty())
                ||
                node->malformed() )
        {
            LinkStatus* ls = new LinkStatus(node, link);
            ls->setAbsoluteUrl(url);

            if(localDomain(ls->absoluteUrl()))
                ls->setExternalDomainDepth(-1);
            else
                ls->setExternalDomainDepth(link->externalDomainDepth() + 1);

            //ls->setIsLocalRestrict(localDomain(url));
            ls->setIsLocalRestrict(ls->local()); // @todo clean this nonsense

            if(!validUrl(url))
                ls->setMalformed(true);

            if(ls->malformed())
                ls->setErrorOccurred(true);

            ls->setOnlyCheckHeader(onlyCheckHeader(ls));

            if(link->externalDomainDepth() > external_domain_depth_)
            {
                kdDebug(23100) <<  "link->externalDomainDepth() > external_domain_depth_: "
                << link->externalDomainDepth() << endl;
                kdDebug(23100) <<  "link: " << endl << link->toString() << endl;
                kdDebug(23100) <<  "child: " << endl << ls->toString() << endl;
            }
            assert(link->externalDomainDepth() <= external_domain_depth_);

            children.push_back(ls);
        }
        if(count == 50)
        {
            kapp->processEvents();
            count = 0;
        }
    }

    return children;
}

bool SearchManager::existUrl(KURL const& url, KURL const& url_parent) const
{
    if(url.prettyURL().isEmpty() || root_.originalUrl() == url.prettyURL())
        return true;

    for(uint i = 0; i != search_results_.size(); ++i)
        for(uint j = 0; j != search_results_[i].size(); ++j)
            for(uint l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                LinkStatus* tmp = search_results_[i][j][l];
                Q_ASSERT(tmp);
                if(tmp->absoluteUrl() == url)
                {
                    tmp->addReferrer(url_parent);
                    kdDebug(23100) << "Another referrer: " << url_parent.url() << endl;
                    return true;
                }
            }

    return false;
}

void SearchManager::startSearch()
{
    assert(current_depth_ == 1);
    assert(search_results_[current_depth_ - 1].size() == 1);
    assert(current_node_ == 0);

    if( (int)current_depth_ <= depth_ || search_mode_ != depth )
        checkVectorLinks(nodeToAnalize());
    else
    {
        kdDebug(23100) <<  "Search Finished! (SearchManager::comecaPesquisa)" << endl;
        finnish();
    }
}

void SearchManager::continueSearch()
{
    assert(not links_being_checked_);

    vector<LinkStatus*> const& no = nodeToAnalize();

    if((uint)current_index_ < no.size())
        checkVectorLinks(no);

    else
    {
        current_index_ = 0;
        kdDebug(23100) <<  "Next node_____________________\n\n";
        ++current_node_;
        if( (uint)current_node_ < (search_results_[current_depth_ - 1]).size() )
            checkVectorLinks(nodeToAnalize());
        else
        {
            kdDebug(23100) <<  "Next Level_____________________________________________________________________________________\n\n\n";
            if( (search_mode_ != SearchManager::depth) ||
                    (current_depth_ < depth_) )
            {
                current_node_ = 0;
                ++current_depth_;

                addLevel();

                if( (uint)current_depth_ == search_results_.size() )
                    checkVectorLinks(nodeToAnalize());
                else
                {
                    kdDebug(23100) <<  "Search Finished! (SearchManager::continueSearch#1)" << endl;
                    finnish();
                }
            }
            else
            {
                kdDebug(23100) <<  "Search Finished! (SearchManager::continueSearch#2)" << endl;
                finnish();
            }
        }
    }
}

vector<LinkStatus*> const& SearchManager::nodeToAnalize() const
{
    assert( (uint)current_depth_ == search_results_.size() );
    assert( (uint)current_node_ < (search_results_[current_depth_ - 1]).size() );

    return (search_results_[current_depth_ - 1])[current_node_];
}

void SearchManager::checkVectorLinks(vector<LinkStatus*> const& links)
{
    checkLinksSimultaneously(chooseLinks(links));
}

vector<LinkStatus*> SearchManager::chooseLinks(vector<LinkStatus*> const& links)
{
    vector<LinkStatus*> escolha;
    for(int i = 0; i != max_simultaneous_connections_; ++i)
    {
        if((uint)current_index_ < links.size())
            escolha.push_back(links[current_index_++]);
    }
    return escolha;
}

void SearchManager::checkLinksSimultaneously(vector<LinkStatus*> const& links)
{
    assert(finished_connections_ <= max_simultaneous_connections_);
    finished_connections_ = 0;
    links_being_checked_ = 0;
    maximum_current_connections_ = -1;

    if(links.size() < (uint)max_simultaneous_connections_)
        maximum_current_connections_ = links.size();
    else
        maximum_current_connections_ = max_simultaneous_connections_;

    for(uint i = 0; i != links.size(); ++i)
    {
        LinkStatus* ls(links[i]);
        assert(ls);

        QString protocol = ls->absoluteUrl().protocol();

        ++links_being_checked_;
        assert(links_being_checked_ <= max_simultaneous_connections_);

        if(ls->malformed())
        {
            assert(ls->errorOccurred());
            assert(ls->error() == "Malformed");

            ls->setChecked(true);
            slotLinkChecked(ls, 0);
        }

        else if(ls->absoluteUrl().prettyURL().contains("javascript:", false))
        {
            ++ignored_links_;
            ls->setIgnored(true);
            ls->setErrorOccurred(true);
            ls->setError("Javascript not suported");
            ls->setChecked(true);
            slotLinkChecked(ls, 0);
        }
        /*
                else if(!(protocol == "http" || protocol == "https"))
                {
                    ++ignored_links_;
                    ls->setIgnored(true);
                    ls->setErrorOccurred(true);
                    ls->setError("Protocol " + protocol + " not suported");
                    ls->setChecked(true);
                    slotLinkChecked(ls, 0);
                }
        */
        else
        {
            LinkChecker* checker = new LinkChecker(ls, time_out_, this, "link_checker");

            connect(checker, SIGNAL(transactionFinished(const LinkStatus *, LinkChecker *)),
                    this, SLOT(slotLinkChecked(const LinkStatus *, LinkChecker *)));
            /*
            			connect(checker, SIGNAL(jobFinnished(LinkChecker *)),
                                this, SLOT(slotLinkCheckerFinnished(LinkChecker *)));
            */
            checker->check();
        }
    }
}

void SearchManager::slotLinkChecked(const LinkStatus * link, LinkChecker * checker)
{
    //kdDebug(23100) <<  "SearchManager::slotLinkChecked -> " << link->absoluteUrl().url() << endl;

    assert(link);
    emit signalLinkChecked(link, checker);
    ++checked_links_;
    ++finished_connections_;
    --links_being_checked_;

    if(links_being_checked_ < 0)
        kdDebug(23100) <<  link->toString() << endl;
    assert(links_being_checked_ >= 0);

    if(canceled_ and searching_ and !links_being_checked_)
    {
        finnish();
    }

    else if(!canceled_ && finished_connections_ == maximumCurrentConnections() )
    {
        continueSearch();
        return;
    }
    /*
            delete checker;
            checker = 0;
    */
}

void SearchManager::addLevel()
{
    search_results_.push_back(vector< vector <LinkStatus*> >());
    vector< vector <LinkStatus*> >& ultimo_nivel(search_results_[search_results_.size() - 2]);

    number_of_level_links_ = 0;
    number_of_links_to_check_ = 0;
    uint end = ultimo_nivel.size();

    for(uint i = 0; i != end; ++i) // nodes
    {
        uint end_sub1 = ultimo_nivel[i].size();
        for(uint j = 0; j != end_sub1; ++j) // links
            ++number_of_level_links_;
    }

    if(number_of_level_links_)
        emit signalAddingLevelTotalSteps(number_of_level_links_);

    for(uint i = 0; i != end; ++i) // nodes
    {
        uint end_sub1 = ultimo_nivel[i].size();
        for(uint j = 0; j != end_sub1; ++j) // links
        {
            vector <LinkStatus*> f(children( LinkStatus::lastRedirection(((ultimo_nivel[i])[j])) ));
            if(f.size() != 0)
            {
                search_results_[search_results_.size() - 1].push_back(f);
                number_of_links_to_check_ += f.size();
            }

            emit signalAddingLevelProgress();
            kapp->processEvents();
        }
    }
    if( (search_results_[search_results_.size() - 1]).size() == 0 )
        search_results_.pop_back();
    else
        emit signalLinksToCheckTotalSteps(number_of_links_to_check_);
}

bool SearchManager::checkable(KURL const& url, LinkStatus const& link_parent) const
{
    if(existUrl(url, link_parent.absoluteUrl()))
        return false;

    if(not checkableByDomain(url, link_parent))
        return false;

    if(not check_parent_dirs_)
    {
        if(::parentDir(root_.absoluteUrl(), url))
            return false;
    }
    if(not check_external_links_)
    {
        if(::externalLink(root_.absoluteUrl(), url))
            return false;
    }

    //kdDebug(23100) <<  "url " << url.url() << " is checkable!" << endl;
    return true;
}

bool SearchManager::checkableByDomain(KURL const& url, LinkStatus const& link_parent) const
{
    bool result = false;
    if(localDomain(url))
        result = true;
    else if( (link_parent.externalDomainDepth() + 1) < external_domain_depth_ )
        result = true;
    else
        result = false;
    /*
        if(!result)
            kdDebug(23100) <<  "\n\nURL " << url.url() << " is not checkable by domain\n\n" << endl;
    */
    return result;
}
/*
bool SearchManager::localDomain(KURL const& url) const
{
    KURL url_root = root_.absoluteUrl();
 
    if(url_root.protocol() != url.protocol())
        return false;
 
    if(url_root.hasHost())
    {
        if(generalDomain())
        {
            return equalHost(domain_, url.host());
        }
        else
        {
            vector<QString> referencia = tokenizeWordsSeparatedBy(domain_, QChar('/'));
            vector<QString> a_comparar = tokenizeWordsSeparatedBy(url.host() + url.directory(), QChar('/'));
 
            if(a_comparar.size() < referencia.size())
                return false;
            else
            {
                for(uint i = 0; i != referencia.size(); ++i)
                {
                    if(i == 0)
                    { // host, deal with specific function
                        if(!equalHost(referencia[i], a_comparar[i], !check_parent_dirs_))
                            return false;
                    }
                    else if(referencia[i] != a_comparar[i])
                        return false;
                }
            }
            return true;
        }
    }
    else if(checkParentDirs())
        return true;
    else
        return url_root.isParentOf(url);
}
*/

/**
	The same as SearchManager::localDomain(), but only for http or https.
	http://linkstatus.paradigma.co.pt != http://paradigma.co.pt
*/
/*
bool SearchManager::isLocalRestrict(KURL const& url) const
{
    assert(url.protocol() == "http" || url.protocol() == "https");
 
    KURL url_root = root_.absoluteUrl();
 
    if(url_root.protocol() != url.protocol())
        return false;
 
    if(url_root.hasHost())
    {
        vector<QString> referencia = tokenizeWordsSeparatedBy(domain_, QChar('/'));
        vector<QString> a_comparar = tokenizeWordsSeparatedBy(url.host() + url.directory(), QChar('/'));
 
        if(a_comparar.size() < referencia.size())
            return false;
        else
        {
            for(uint i = 0; i != referencia.size(); ++i)
            {
                if(i == 0)
                { // host, deal with specific function
                    if(!equalHost(referencia[i], a_comparar[i], true))
                        return false;
                }
                else if(referencia[i] != a_comparar[i])
                    return false;
            }
        }
        return true;
    }
    else
        return false;
}
*/
bool SearchManager::generalDomain() const
{
    if(checked_general_domain_)
        return general_domain_;

    else
    {
        assert(!domain_.isEmpty());

        if(!check_parent_dirs_)
            return false;

        int barra = domain_.find('/');
        if(barra != -1 && (uint)barra != domain_.length() - 1)
        {
            kdDebug(23100) <<  "Domain nao vago" << endl;
            return false;
        }
        else
        {
            vector<QString> palavras = tokenizeWordsSeparatedByDots(domain_);
            assert(palavras.size() >= 1); // host might be localhost

            QString primeira_palavra = palavras[0];
            if(primeira_palavra == "www")
            {
                assert(palavras.size() >= 3);
                kdDebug(23100) <<  "Domain vago" << endl;
                return true;
            }
            else if(palavras.size() == 2)
            {
                kdDebug(23100) <<  "Domain vago" << endl;
                return true;
            }
            else
            {
                kdDebug(23100) <<  "Domain nao vago" << endl;
                return false;
            }
        }
    }
}

bool SearchManager::onlyCheckHeader(LinkStatus* ls) const
{
    if(search_mode_ == depth)
        return current_depth_ == depth_;

    else if(search_mode_ == domain)
        return not ls->local() and
               ls->externalDomainDepth() == external_domain_depth_ - 1;

    else
        return
            current_depth_ == depth_ or
            (not ls->local() and
             ls->externalDomainDepth() == external_domain_depth_ - 1);
}

void SearchManager::slotSearchFinished()
{}

void SearchManager::slotLinkCheckerFinnished(LinkChecker * checker)
{
    kdDebug(23100) <<  "deleting linkchecker" << endl;

    assert(checker);
    //assert(checker->linkStatus()->checked());

    delete checker;
    checker = 0;
}

#include "searchmanager.moc"
