/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
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

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <khtml_part.h>
#include <kprotocolmanager.h>

#include <qstring.h>
#include <qvaluevector.h>
#include <qdom.h>

#include <iostream>
#include <unistd.h>

#include "searchmanager.h"
#include "../parser/mstring.h"
#include "../cfg/klsconfig.h"


SearchManager::SearchManager(int max_simultaneous_connections, int time_out,
                             QObject *parent, const char *name)
        : QObject(parent, name),
        max_simultaneous_connections_(max_simultaneous_connections), has_document_root_(false), 
        depth_(-1), current_depth_(0), external_domain_depth_(0),
        current_node_(0), current_index_(0), links_being_checked_(0),
        finished_connections_(max_simultaneous_connections_),
        maximum_current_connections_(-1), general_domain_(false),
        checked_general_domain_(false), time_out_(time_out), current_connections_(0),
        send_identification_(true), canceled_(false), searching_(false), checked_links_(0), ignored_links_(0),
        check_parent_dirs_(true), check_external_links_(true), check_regular_expressions_(false),
        number_of_level_links_(0), number_of_links_to_check_(0)
{
    root_.setIsRoot(true);

    if (KLSConfig::userAgent().isEmpty()) {
        KLSConfig::setUserAgent(KProtocolManager::defaultUserAgent());
    }
    user_agent_ = KLSConfig::userAgent();
}

void SearchManager::reset()
{
    kdDebug(23100) <<  "SearchManager::reset()" << endl;

    //Q_ASSERT(not links_being_checked_);

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
    check_regular_expressions_ = false;
    current_connections_ = 0;
    canceled_ = false;
    searching_ = false;
    checked_links_ = 0;    
    if(KLSConfig::userAgent().isEmpty()) {
        KLSConfig::setUserAgent(KProtocolManager::defaultUserAgent());
    }
    user_agent_ = KLSConfig::userAgent();

    removeHtmlParts();
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

    Q_ASSERT(root.isValid());
    //Q_ASSERT(root.protocol() == "http" || root.protocol() == "https");

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
        Q_ASSERT(depth_ != -1);
    else if(modo == domain)
        Q_ASSERT(depth_ == -1);
    else
        Q_ASSERT(depth_ != -1);

    searching_ = true;

    //Q_ASSERT(domain_ != QString::null);
    checkRoot();
}

void SearchManager::resume()
{
    searching_ = true;
    canceled_ = false;
    continueSearch();
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

void SearchManager::pause()
{
    searching_ = false;
    while(links_being_checked_)
    {
        kdDebug(23100) <<  "links_being_checked_: " << links_being_checked_ << endl;
        sleep(1);
    }
    emit signalSearchPaused();
}

void SearchManager::cancelSearch()
{
    canceled_ = true;
}

void SearchManager::checkRoot()
{
    LinkChecker* checker = new LinkChecker(&root_, time_out_, this, "link_checker");
    checker->setSearchManager(this);

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
    kdDebug(23100) <<  "SearchManager::slotRootChecked:" << endl;
    kdDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
            LinkStatus::lastRedirection(&root_)->absoluteUrl().url() << endl;

    Q_ASSERT(checked_links_ == 0);
    Q_ASSERT(search_results_.size() == 0);

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
        Q_ASSERT(search_results_.size() == 1);

        if(no.size() > 0)
        {
            startSearch();
        }
        else
        {
            kdDebug(23100) <<  "SearchManager::slotRootChecked#1" << endl;
            finnish();
        }
    }

    else
    {
        Q_ASSERT(search_results_.size() == 0);
        kdDebug(23100) <<  "SearchManager::slotRootChecked#2" << endl;
        finnish();
    }

    delete checker;
    checker = 0;
}

vector<LinkStatus*> SearchManager::children(LinkStatus* link)
{
    vector<LinkStatus*> children;

    if(!link || link->absoluteUrl().hasRef())
        return children;

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
            url = Url::normalizeUrl(node->url(), *link, documentRoot().path());

        if( (node->isLink() &&
                checkable(url, *link) &&
                !Url::existUrl(url, children) &&
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

            if(!validUrl(url)) {
                ls->setMalformed(true);
                ls->setErrorOccurred(true); 
            }

            ls->setOnlyCheckHeader(onlyCheckHeader(ls));

            if(link->externalDomainDepth() > external_domain_depth_)
            {
                kdDebug(23100) <<  "link->externalDomainDepth() > external_domain_depth_: "
                << link->externalDomainDepth() << endl;
                kdDebug(23100) <<  "link: " << endl << link->toString() << endl;
                kdDebug(23100) <<  "child: " << endl << ls->toString() << endl;
            }
            Q_ASSERT(link->externalDomainDepth() <= external_domain_depth_);

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
                { // URL exists
                    QValueVector<KURL> referrers(tmp->referrers());

                    // Add new referrer
                    for(uint i = 0; i != referrers.size(); ++i)
                    {
                        if(referrers[i] == url_parent)
                            return true;
                    }
                    tmp->addReferrer(url_parent);
                    
                    return true;
                }
            }

    return false;
}

LinkStatus const* SearchManager::linkStatus(QString const& s_url) const
{
    Q_ASSERT(!s_url.isEmpty());

    if(root_.absoluteUrl().url() == s_url)
        return &root_;

    int count = 0;
    for(uint i = 0; i != search_results_.size(); ++i)
        for(uint j = 0; j != search_results_[i].size(); ++j)
            for(uint l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                ++count;

                LinkStatus* ls = search_results_[i][j][l];
                Q_ASSERT(ls);
                if(ls->absoluteUrl().url() == s_url && ls->checked())
                    return ls;

                if(count == 50)
                {
                    count = 0;
                    kapp->processEvents();
                }

            }

    return 0;
}


void SearchManager::startSearch()
{
    Q_ASSERT(current_depth_ == 1);
    Q_ASSERT(search_results_[current_depth_ - 1].size() == 1);
    Q_ASSERT(current_node_ == 0);

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
    Q_ASSERT(!links_being_checked_);

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
            if(search_mode_ == SearchManager::domain ||
                    current_depth_ < depth_)
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
    Q_ASSERT( (uint)current_depth_ == search_results_.size() );
    Q_ASSERT( (uint)current_node_ < (search_results_[current_depth_ - 1]).size() );

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
    Q_ASSERT(finished_connections_ <= max_simultaneous_connections_);
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
        Q_ASSERT(ls);

        QString protocol = ls->absoluteUrl().protocol();

        ++links_being_checked_;
        Q_ASSERT(links_being_checked_ <= max_simultaneous_connections_);

        if(ls->malformed())
        {
            Q_ASSERT(ls->errorOccurred());
            Q_ASSERT(ls->status() == LinkStatus::MALFORMED);

            ls->setChecked(true);
            slotLinkChecked(ls, 0);
        }

        else if(ls->absoluteUrl().prettyURL().contains("javascript:", false))
        {
            ++ignored_links_;
            ls->setIgnored(true);
            ls->setErrorOccurred(true);
            ls->setError(i18n( "Javascript not supported" ));
            ls->setStatus(LinkStatus::NOT_SUPPORTED);
            ls->setChecked(true);
            slotLinkChecked(ls, 0);
        }
        /*
                else if(!(protocol == "http" || protocol == "https"))
                {
                    ++ignored_links_;
                    ls->setIgnored(true);
                    ls->setErrorOccurred(true);
                    ls->setError(i18n("Protocol %1 not supported").arg(protocol));
                    ls->setStatus(LinkStatus::MALFORMED);
                    ls->setChecked(true);
                    slotLinkChecked(ls, 0);
                }
        */
        else
        {
            LinkChecker* checker = new LinkChecker(ls, time_out_, this, "link_checker");
            checker->setSearchManager(this);

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
    kdDebug(23100) <<  "SearchManager::slotLinkChecked:" << endl;
//     kdDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
//             LinkStatus::lastRedirection((const_cast<LinkStatus*> (link)))->absoluteUrl().url() << endl;

    Q_ASSERT(link);
    emit signalLinkChecked(link, checker);
    ++checked_links_;
    ++finished_connections_;
    --links_being_checked_;

    if(links_being_checked_ < 0)
        kdDebug(23100) <<  link->toString() << endl;
    Q_ASSERT(links_being_checked_ >= 0);

    if(canceled_ && searching_ && !links_being_checked_)
    {
        pause();
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
//             kapp->processEvents();
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

    if(!checkableByDomain(url, link_parent))
        return false;

    if(!check_parent_dirs_)
    {
        if(Url::parentDir(root_.absoluteUrl(), url))
            return false;
    }
    if(!check_external_links_)
    {
        if(Url::externalLink(root_.absoluteUrl(), url))
            return false;
    }
    if(check_regular_expressions_)
    {
        Q_ASSERT(!reg_exp_.isEmpty());

        if(reg_exp_.search(url.url()) != -1)
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
    Q_ASSERT(url.protocol() == "http" || url.protocol() == "https");
 
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
        Q_ASSERT(!domain_.isEmpty());

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
            Q_ASSERT(palavras.size() >= 1); // host might be localhost

            QString primeira_palavra = palavras[0];
            if(primeira_palavra == "www")
            {
                Q_ASSERT(palavras.size() >= 3);
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
        return !ls->local() &&
               ls->externalDomainDepth() == external_domain_depth_ - 1;

    else
        return
            current_depth_ == depth_ ||
            (!ls->local() &&
             ls->externalDomainDepth() == external_domain_depth_ - 1);
}

void SearchManager::slotSearchFinished()
{}

void SearchManager::slotLinkCheckerFinnished(LinkChecker * checker)
{
    kdDebug(23100) <<  "deleting linkchecker" << endl;

    Q_ASSERT(checker);
    //Q_ASSERT(checker->linkStatus()->checked());

    delete checker;
    checker = 0;
}

KHTMLPart* SearchManager::htmlPart(QString const& key_url) const
{
    if(!html_parts_.contains(key_url))
        return 0;

    return html_parts_[key_url];
}

void SearchManager::addHtmlPart(QString const& key_url, KHTMLPart* html_part)
{
    Q_ASSERT(!key_url.isEmpty());
    Q_ASSERT(html_part);

    // FIXME configurable
    if(html_parts_.count() > 150)
        removeHtmlParts();

    html_parts_.insert(key_url, html_part);
}

void SearchManager::removeHtmlParts()
{
    KHTMLPartMap::Iterator it;
    for(it = html_parts_.begin(); it != html_parts_.end(); ++it) 
    {
        delete it.data();
        it.data() = 0;
    }

    html_parts_.clear();
}

void SearchManager::save(QDomElement& element) const
{
    // <url>
    QDomElement child_element = element.ownerDocument().createElement("url");
    child_element.appendChild(element.ownerDocument().createTextNode(root_.absoluteUrl().prettyURL()));
    element.appendChild(child_element);

    // <recursively>
    bool recursively = searchMode() == domain || depth_ > 0;
    child_element = element.ownerDocument().createElement("recursively");
    child_element.appendChild(element.ownerDocument().createTextNode(recursively ? "true" : "false"));
    element.appendChild(child_element);

    // <depth>
    child_element = element.ownerDocument().createElement("depth");
    child_element.appendChild(element.ownerDocument().
            createTextNode(searchMode() == domain ? QString("Unlimited") : QString::number(depth_)));
    element.appendChild(child_element);

    // <check_parent_folders>
    child_element = element.ownerDocument().createElement("check_parent_folders");
    child_element.appendChild(element.ownerDocument().
            createTextNode(checkParentDirs() ? "true" : "false"));
    element.appendChild(child_element);

    // <check_external_links>
    child_element = element.ownerDocument().createElement("check_external_links");
    child_element.appendChild(element.ownerDocument().
            createTextNode(checkExternalLinks() ? "true" : "false"));
    element.appendChild(child_element);

    // <check_regular_expression>
    child_element = element.ownerDocument().createElement("check_regular_expression");
    child_element.setAttribute("check", checkRegularExpressions() ? "true" : "false");
    if(checkRegularExpressions())
        child_element.appendChild(element.ownerDocument().
                createTextNode(reg_exp_.pattern()));
    element.appendChild(child_element);
    
    child_element = element.ownerDocument().createElement("link_list");
    element.appendChild(child_element);
    
    for(uint i = 0; i != search_results_.size(); ++i)
    {
        for(uint j = 0; j != search_results_[i].size() ; ++j)
        {
            for(uint l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                LinkStatus* ls = ((search_results_[i])[j])[l];
                if(ls->checked())
                    ls->save(child_element);
            }
        }
    } 
}

QString SearchManager::toXML() const
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction( "xml", 
                                       "version=\"1.0\" encoding=\"UTF-8\""));
    
    QDomElement root = doc.createElement("klinkstatus");
    doc.appendChild(root);

    save(root);
    
    return doc.toString(4);
}

#include "searchmanager.moc"
