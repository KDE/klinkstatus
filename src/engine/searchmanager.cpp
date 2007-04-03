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
#include <khtml_part.h>
#include <kprotocolmanager.h>

#include <QString>
#include <q3valuelist.h>
#include <qdom.h>
#include <threadweaver/ThreadWeaver.h>

#include <iostream>
#include <unistd.h>

#include "searchmanager.h"
#include "linkstatushelper.h"
#include "parser/mstring.h"
#include "klsconfig.h"


SearchManager::SearchManager(int max_simultaneous_connections, int time_out,
                             QObject *parent)
        : QObject(parent), 
        max_simultaneous_connections_(max_simultaneous_connections), has_document_root_(false), 
        depth_(-1), current_depth_(0), external_domain_depth_(0),
        current_node_(0), current_index_(0), links_being_checked_(0),
        finished_connections_(max_simultaneous_connections_),
        maximum_current_connections_(-1), general_domain_(false),
        checked_general_domain_(false), time_out_(time_out), current_connections_(0),
        send_identification_(true), canceled_(false), searching_(false), checked_links_(0), ignored_links_(0),
        check_parent_dirs_(true), check_external_links_(true), check_regular_expressions_(false),
        number_of_current_level_links_(0), number_of_new_links_to_check_(0), m_addLevelJob(0)
{
    root_.setIsRoot(true);

    connect(Weaver::instance(), SIGNAL(jobDone(Job*)), SLOT(slotJobDone(Job*)));
}

void SearchManager::reset()
{
    kDebug(23100) <<  "SearchManager::reset()" << endl;

    //Q_ASSERT(not links_being_checked_);

    LinkStatusHelper::reset(&root_);
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
                    kDebug(23100) <<  "LinkStatus NULL!!" << endl;
            }
            search_results_[i][j].clear();
        }
        search_results_[i].clear();
    }
    search_results_.clear();
    kDebug(23100) <<  endl;
}

void SearchManager::startSearch(KUrl const& root, SearchMode const& modo)
{
    canceled_ = false;

    Q_ASSERT(root.isValid());

    if(root.hasHost() && (domain_.isNull() || domain_.isEmpty()))
    {
        setDomain(root.host() + root.directory());
        kDebug(23100) << "Domain: " << domain_ << endl;
    }
    root_.setIsRoot(true);
    root_.setDepth(0);
    root_.setOriginalUrl(root.prettyUrl());
    root_.setAbsoluteUrl(root);
    root_.setOnlyCheckHeader(false);
    root_.setRootUrl(root);

    search_results_hash_.insert(root_.absoluteUrl(), &root_);

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
    while(links_being_checked_)
    {
        kDebug(23100) <<  "links_being_checked_: " << links_being_checked_ << endl;
        sleep(1);
    }
    searching_ = false;
    emit signalSearchFinished();
}

void SearchManager::pause()
{
    while(links_being_checked_)
    {
        kDebug(23100) <<  "links_being_checked_: " << links_being_checked_ << endl;
        sleep(1);
    }
    searching_ = false;
    emit signalSearchPaused();
}

void SearchManager::cancelSearch()
{
    canceled_ = true;
}

void SearchManager::checkRoot()
{
    LinkChecker* checker = new LinkChecker(&root_, time_out_, this);
    checker->setSearchManager(this);

    connect(checker, SIGNAL(transactionFinished(LinkStatus*, LinkChecker*)),
            this, SLOT(slotRootChecked(LinkStatus*, LinkChecker*)));

    checker->check();
}

void SearchManager::slotRootChecked(LinkStatus* link, LinkChecker* checker)
{
    kDebug(23100) <<  "SearchManager::slotRootChecked:" << endl;
    kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
            LinkStatusHelper::lastRedirection(&root_)->absoluteUrl().url() << endl;

    Q_ASSERT(checked_links_ == 0);
    Q_ASSERT(search_results_.size() == 0);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
        LinkStatusHelper::validateMarkup(link);
    
    ++checked_links_;
    //kDebug(23100) <<  "++checked_links_: SearchManager::slotRootChecked" << endl;
    emit signalRootChecked(link);
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection());


    if(search_mode_ != depth || depth_ > 0)
    {
        current_depth_ = 1;

        vector<LinkStatus*> node;
        fillWithChildren(LinkStatusHelper::lastRedirection(&root_), node);

        emit signalLinksToCheckTotalSteps(node.size());

        vector< vector<LinkStatus*> > nivel;
        nivel.push_back(node);

        search_results_.push_back(nivel);

        if(search_results_.size() != 1)
        {
            kDebug(23100) <<  "search_results_.size() != 1:" << endl;
            kDebug(23100) <<  "size: " << search_results_.size() << endl;
        }
        Q_ASSERT(search_results_.size() == 1);

        if(node.size() > 0)
        {
            startSearch();
        }
        else
        {
        kDebug(23100) <<  "SearchManager::slotRootChecked#1" << endl;
            finnish();
        }
    }

    else
    {
        Q_ASSERT(search_results_.size() == 0);
        kDebug(23100) <<  "SearchManager::slotRootChecked#2" << endl;
        finnish();
    }

    checker->deleteLater();
}

void SearchManager::fillWithChildren(LinkStatus* link, vector<LinkStatus*>& children)
{
    if(!link || link->absoluteUrl().hasRef())
        return;

    vector<Node*> const& nodes = link->childrenNodes();
    children.reserve(nodes.size());

    QHash<KUrl, LinkStatus*> children_hash;
    children_hash.reserve(nodes.size());
    
    for(uint i = 0; i != nodes.size(); ++i)
    {
        Node* node = nodes[i];
        KUrl url;
        if(node->url().isEmpty())
            url = "";
        else
            url = Url::normalizeUrl(node->url(), *link, documentRoot().path());

        if( (node->isLink() &&
                checkable(url, *link) &&
                !children_hash.contains(url) &&
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
                kDebug(23100) <<  "link->externalDomainDepth() > external_domain_depth_: "
                << link->externalDomainDepth() << endl;
                kDebug(23100) <<  "link: " << endl << LinkStatusHelper::toString(link) << endl;
                kDebug(23100) <<  "child: " << endl << LinkStatusHelper::toString(ls) << endl;

                Q_ASSERT(false);
            }
//             Q_ASSERT(link->externalDomainDepth() <= external_domain_depth_);

            children.push_back(ls);
            children_hash.insert(url, ls);
        }
    }
            
    search_results_hash_ = search_results_hash_.unite(children_hash);
}

bool SearchManager::existUrl(KUrl const& url, KUrl const& url_parent) const
{
    if(url.prettyUrl().isEmpty() || root_.originalUrl() == url.prettyUrl())
        return true;

    LinkStatus* ls = search_results_hash_.value(url, 0);
    if(ls) {      
        // Add new referrer
        if(!ls->referrers().contains(url_parent))
            ls->addReferrer(url_parent);

        return true;
    }
    else {
      return false;
    }
}

LinkStatus const* SearchManager::linkStatus(QString const& s_url) const
{
    Q_ASSERT(!s_url.isEmpty());

    if(root_.absoluteUrl().url() == s_url)
        return &root_;

    return search_results_hash_.value(KUrl(s_url), 0);
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
        kDebug(23100) <<  "Search Finished! (SearchManager::comecaPesquisa)" << endl;
        finnish();
    }
}

void SearchManager::slotLevelAdded()
{
    if( (uint)current_depth_ == search_results_.size() )
        checkVectorLinks(nodeToAnalize());
    else
    {
        kDebug(23100) <<  "Search Finished! (SearchManager::continueSearch#1)" << endl;
        finnish();
    }
}

void SearchManager::continueSearch()
{
    Q_ASSERT(!links_being_checked_);

    vector<LinkStatus*> const& node = nodeToAnalize();

    if((uint)current_index_ < node.size())
        checkVectorLinks(node);

    else
    {
        current_index_ = 0;
        kDebug(23100) <<  "Next node_____________________\n\n";
        ++current_node_;
        if( (uint)current_node_ < (search_results_[current_depth_ - 1]).size() )
            checkVectorLinks(nodeToAnalize());
        else
        {
            kDebug(23100) <<  "Next Level_____________________________________________________________________________________\n\n\n";
            if(search_mode_ == SearchManager::domain ||
                    current_depth_ < depth_)
            {
                current_node_ = 0;
                ++current_depth_;

                m_addLevelJob = new AddLevelJob(*this);
                Weaver::instance()->enqueue(m_addLevelJob);
            }
            else
            {
                kDebug(23100) <<  "Search Finished! (SearchManager::continueSearch#2)" << endl;
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

        else if(ls->absoluteUrl().prettyUrl().contains("javascript:", Qt::CaseInsensitive))
        {
            ++ignored_links_;
            ls->setIgnored(true);
            ls->setErrorOccurred(true);
            ls->setError("Javascript not supported");
            ls->setStatus(LinkStatus::NOT_SUPPORTED);
            ls->setChecked(true);
            slotLinkChecked(ls, 0);
        }
        else
        {
            LinkChecker* checker = new LinkChecker(ls, time_out_, this);
            checker->setSearchManager(this);

            connect(checker, SIGNAL(transactionFinished(LinkStatus*, LinkChecker*)),
                    this, SLOT(slotLinkChecked(LinkStatus*, LinkChecker*)));

            checker->check();
        }
    }
}

void SearchManager::recheckLink(KUrl const& url)
{
    LinkStatus* ls = search_results_hash_.value(url, 0);
    Q_ASSERT(ls);

    LinkStatusHelper::resetResults(ls);
    
    LinkChecker* checker = new LinkChecker(ls, time_out_, this);
    checker->setSearchManager(this);

    connect(checker, SIGNAL(transactionFinished(LinkStatus*, LinkChecker*)),
            this, SLOT(slotLinkRechecked(LinkStatus*, LinkChecker*)));

    checker->check();
}

void SearchManager::linkRedirectionChecked(LinkStatus* link)
{
    emit signalLinkChecked(link);
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection());
}

void SearchManager::slotLinkChecked(LinkStatus* link, LinkChecker* checker)
{
    kDebug(23100) <<  "SearchManager::slotLinkChecked: " << checked_links_ << endl;
//     kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
//             LinkStatus::lastRedirection((const_cast<LinkStatus*> (link)))->absoluteUrl().url() << endl;

    Q_ASSERT(link);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
      LinkStatusHelper::validateMarkup(link);

    emit signalLinkChecked(link);
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection());
    
    ++checked_links_;
    ++finished_connections_;
    --links_being_checked_;
  
    if(links_being_checked_ < 0)
        kDebug(23100) <<  LinkStatusHelper::toString(link) << endl;
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
    checker->deleteLater();
}

void SearchManager::slotLinkRechecked(LinkStatus* link, LinkChecker* checker)
{
    kDebug(23100) <<  "SearchManager::slotLinkRechecked" << endl;
//     kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
//             LinkStatus::lastRedirection((const_cast<LinkStatus*> (link)))->absoluteUrl().url() << endl;

    Q_ASSERT(link);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
      LinkStatusHelper::validateMarkup(link);

    emit signalLinkRechecked(link);
      
    checker->deleteLater();
}

void SearchManager::addLevel()
{
    // add the new empty level
    search_results_.push_back(vector< vector <LinkStatus*> >());
    // keep the reference to it
    vector< vector <LinkStatus*> >& new_level(search_results_[search_results_.size() - 1]);

    // keep the reference to the current level
    vector< vector <LinkStatus*> >& current_level(search_results_[search_results_.size() - 2]);

    // To signal the progress of add level task. For each link, all children have to be find
    number_of_current_level_links_ = 0;
    // number of new link to check in the new level
    number_of_new_links_to_check_ = 0;
    uint current_level_number_of_nodes = current_level.size();

    // Count all the links in the level before, so progress can be signalized
    for(uint i = 0; i != current_level_number_of_nodes; ++i) // nodes
    {
        uint node_size = current_level[i].size();
        for(uint j = 0; j != node_size; ++j) // links
            ++number_of_current_level_links_;
    }
    new_level.reserve(number_of_current_level_links_);

    if(number_of_current_level_links_ != 0)
        emit signalAddingLevelTotalSteps(number_of_current_level_links_);

    for(uint i = 0; i != current_level_number_of_nodes; ++i) // nodes
    {
        uint node_size = current_level[i].size();
        for(uint j = 0; j != node_size; ++j) // links
        {
            vector<LinkStatus*>& node = current_level[i];
            LinkStatus* linkstatus = node[j];
            linkstatus = LinkStatusHelper::lastRedirection(linkstatus);
            vector <LinkStatus*> new_node;
            fillWithChildren(linkstatus, new_node);
            if(new_node.size() != 0)
            {
                // Push node
                new_level.push_back(new_node);
                number_of_new_links_to_check_ += new_node.size();
            }

            emit signalAddingLevelProgress();
        }
    }
    if(new_level.size() == 0)
        search_results_.pop_back();
    else 
        emit signalLinksToCheckTotalSteps(number_of_new_links_to_check_);
}

bool SearchManager::checkable(KUrl const& url, LinkStatus const& link_parent) const
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

        if(reg_exp_.indexIn(url.url()) != -1)
            return false;
    }

    //kDebug(23100) <<  "url " << url.url() << " is checkable!" << endl;
    return true;
}

bool SearchManager::checkableByDomain(KUrl const& url, LinkStatus const& link_parent) const
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
            kDebug(23100) <<  "\n\nURL " << url.url() << " is not checkable by domain\n\n" << endl;
    */
    return result;
}

bool SearchManager::generalDomain() const
{
    if(checked_general_domain_)
        return general_domain_;

    else
    {
        Q_ASSERT(!domain_.isEmpty());

        if(!check_parent_dirs_)
            return false;

        int barra = domain_.indexOf('/');
        if(barra != -1 && barra != domain_.length() - 1)
        {
            kDebug(23100) <<  "Domain nao vago" << endl;
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
                kDebug(23100) <<  "Domain vago" << endl;
                return true;
            }
            else if(palavras.size() == 2)
            {
                kDebug(23100) <<  "Domain vago" << endl;
                return true;
            }
            else
            {
                kDebug(23100) <<  "Domain nao vago" << endl;
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
        delete it.value();
        it.value() = 0;
    }

    html_parts_.clear();
}

void SearchManager::save(QDomElement& element) const
{
    // <url>
    QDomElement child_element = element.ownerDocument().createElement("url");
    child_element.appendChild(element.ownerDocument().createTextNode(root_.absoluteUrl().prettyUrl()));
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
                    LinkStatusHelper::save(ls, child_element);
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

// WARNING If there are several SearchManagers, they receive job done from all of them
void SearchManager::slotJobDone(Job* job)
{
    if(job != m_addLevelJob)
        return;  

    slotLevelAdded();
}


AddLevelJob::AddLevelJob(SearchManager& manager)
  : m_searchManager(manager)
{
}

AddLevelJob::~AddLevelJob()
{
}
    
void AddLevelJob::run()
{
    m_searchManager.addLevel();
}

#include "searchmanager.moc"
