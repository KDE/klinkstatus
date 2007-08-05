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

#include "searchmanager.h"

#include <kapplication.h>
#include <kdebug.h>
#include <khtml_part.h>
#include <kprotocolmanager.h>

#include <QString>
#include <qdom.h>
#include <QTimer>
    
#include <iostream>
#include <unistd.h>

#include "parser/mstring.h"
#include "klsconfig.h"


SearchManager::SearchManager(int max_simultaneous_connections, int time_out,
                             QObject *parent)
        : QObject(parent), recheck_mode_(false), 
        max_simultaneous_connections_(max_simultaneous_connections), has_document_root_(false), 
        depth_(-1), current_depth_(0), external_domain_depth_(0),
        current_node_(0), current_index_(0), links_being_checked_(0),
        finished_connections_(max_simultaneous_connections_),
        maximum_current_connections_(-1), general_domain_(false),
        checked_general_domain_(false), time_out_(time_out), current_connections_(0),
        send_identification_(true), canceled_(false), searching_(false), checked_links_(0), ignored_links_(0),
        check_parent_dirs_(true), check_external_links_(true), check_regular_expressions_(false),
        number_of_current_level_links_(0), 
        links_rechecked_(0), recheck_current_index_(0)
{
    kDebug(23100) <<  "SearchManager::SearchManager()";

    root_.setIsRoot(true);

    m_weaver.setMaximumNumberOfThreads(10);
    connect(&m_weaver, SIGNAL(jobDone(ThreadWeaver::Job*)), SLOT(slotJobDone(ThreadWeaver::Job*)));
}

void SearchManager::reset()
{
    kDebug(23100) <<  "SearchManager::reset()";

    //Q_ASSERT(not links_being_checked_);

    LinkStatusHelper::reset(&root_);
    cleanItems();
    recheck_mode_ = false;
    recheck_links_.clear();
    links_rechecked_ = 0;
    recheck_current_index_ = 0;
    search_results_hash_.clear();
    new_level_.clear();
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
    kDebug(23100) <<  "SearchManager::~SearchManager()";
    reset();
}

void SearchManager::cleanItems()
{
    for(int i = 0; i != search_results_.size(); ++i)
    {
        for(int j = 0; j != search_results_[i].size() ; ++j)
        {
            for(int l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                if(((search_results_[i])[j])[l] != 0)
                {
                    delete ((search_results_[i])[j])[l];
                    ((search_results_[i])[j])[l] = 0;
                }
                else
                    kDebug(23100) <<  "LinkStatus NULL!!";
            }
            search_results_[i][j].clear();
        }
        search_results_[i].clear();
    }
    search_results_.clear();
    kDebug(23100);
}

void SearchManager::recheckLinks(QList<LinkStatus*> const& linkstatus_list)
{
    kDebug(23100) << "SearchManager::recheckLinks: " << linkstatus_list.size();
  
    Q_ASSERT(!searching_);
    Q_ASSERT(checked_links_ >= linkstatus_list.size());
    Q_ASSERT(search_results_.size() != 0);

    recheck_mode_ = true;
    canceled_ = false;
    searching_ = true;
    links_rechecked_ = 0;
    recheck_current_index_ = 0;
    
    recheck_links_.clear();
//     recheck_links_.reserve(linkstatus_list.size());
    recheck_links_ = linkstatus_list;

    if(recheck_links_.size() == 0) {
        finnish();
        return;
    }
    
    for(int i = 0; i != recheck_links_.size(); ++i) {
        LinkStatus* ls = recheck_links_[i];
        Q_ASSERT(ls);
        LinkStatusHelper::resetResults(ls);
    }

    emit signalLinksToCheckTotalSteps(recheck_links_.size());

    checkVectorLinksToRecheck(recheck_links_);
}

void SearchManager::startSearch(KUrl const& root, SearchMode const& modo)
{
    kDebug(23100) <<  "SearchManager::startSearch()";

    root_url_ = root;
    canceled_ = false;

    Q_ASSERT(root.isValid());

    if(root.hasHost() && (domain_.isNull() || domain_.isEmpty()))
    {
        setDomain(root.host() + root.directory());
        kDebug(23100) << "Domain: " << domain_;
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
    kDebug(23100) << "SearchManager::resume";

    searching_ = true;
    canceled_ = false;
    if(recheck_mode_)
        continueRecheck();
    else
        continueSearch();
}

void SearchManager::finnish()
{
    if(links_being_checked_ || m_weaver.queueLength() != 0)
    {
        kDebug(23100) << "Waiting for links being checked: " << links_being_checked_;
        QTimer::singleShot(500, this, SLOT(finnish()));
        return;
    }
    kDebug(23100) << "SearchManager::finnish";
    if(!recheck_mode_)
        kDebug(23100) << "Links Checked: " << checked_links_;
    else
        kDebug(23100) << "Links Rechecked: " << links_rechecked_;

    searching_ = false;
    emit signalSearchFinished(this);
}

void SearchManager::pause()
{
    kDebug(23100) <<  "SearchManager::pause()";
    
    while(links_being_checked_)
    {
        kDebug(23100) <<  "SearchManager::pause()" << endl
            << "waiting for links being checked: "
            << links_being_checked_ << endl;
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
    kDebug(23100) <<  "SearchManager::slotRootChecked:";
    kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
            LinkStatusHelper::lastRedirection(&root_)->absoluteUrl().url() << endl;

    Q_ASSERT(checked_links_ == 0);
    Q_ASSERT(search_results_.size() == 0);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
        LinkStatusHelper::validateMarkup(link);
    
    ++checked_links_;
    //kDebug(23100) <<  "++checked_links_: SearchManager::slotRootChecked";
    emit signalRootChecked(link);
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection());


    if(search_mode_ != depth || depth_ > 0)
    {
        current_depth_ = 1;

        QList<LinkStatus*> node;
        fillWithChildren(LinkStatusHelper::lastRedirection(&root_), node);

        emit signalLinksToCheckTotalSteps(node.size());

        QList< QList<LinkStatus*> > nivel;
        nivel.push_back(node);

        search_results_.push_back(nivel);

        if(search_results_.size() != 1)
        {
            kDebug(23100) <<  "search_results_.size() != 1:";
            kDebug(23100) <<  "size: " << search_results_.size();
        }
        Q_ASSERT(search_results_.size() == 1);

        if(node.size() > 0)
        {
            startSearchAfterRoot();
        }
        else
        {
            kDebug(23100) <<  "SearchManager::slotRootChecked#1";
            finnish();
        }
    }

    else
    {
        Q_ASSERT(search_results_.size() == 0);
        kDebug(23100) <<  "SearchManager::slotRootChecked#2";
        finnish();
    }

    checker->deleteLater();
}

void SearchManager::fillWithChildren(LinkStatus* link, QList<LinkStatus*>& children)
{
    if(!link || link->absoluteUrl().hasRef())
        return;

    QList<Node*> const& nodes = link->childrenNodes();
//     children.reserve(nodes.size());

    QHash<KUrl, LinkStatus*> children_hash;
    children_hash.reserve(nodes.size());
    
    for(int i = 0; i != nodes.size(); ++i)
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
                kDebug(23100) <<  "link: " << endl << LinkStatusHelper::toString(link);
                kDebug(23100) <<  "child: " << endl << LinkStatusHelper::toString(ls);

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
        // Add new referrer. If exists, do nothing (QSet)
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

void SearchManager::startSearchAfterRoot()
{
    kDebug(23100) <<  "SearchManager::startSearch() | after root checked";
    
    Q_ASSERT(current_depth_ == 1);
    Q_ASSERT(search_results_[current_depth_ - 1].size() == 1);
    Q_ASSERT(current_node_ == 0);

    if( (int)current_depth_ <= depth_ || search_mode_ != depth )
        checkVectorLinks(nodeToAnalize());
    else
    {
        kDebug(23100) <<  "Search Finished! (SearchManager::comecaPesquisa)";
        finnish();
    }
}

void SearchManager::slotLevelAdded()
{
    kDebug(23100) << "SearchManager::slotLevelAdded";

    if(current_depth_ == search_results_.size() )
        checkVectorLinks(nodeToAnalize());
    else
    {
        kDebug(23100) <<  "Search Finished! (SearchManager::continueSearch#1)";
        finnish();
    }
}

void SearchManager::continueRecheck()
{
    kDebug(23100) << "SearchManager::continueRecheck";
    checkVectorLinksToRecheck(recheck_links_);
}

void SearchManager::continueSearch()
{
    kDebug(23100) << "SearchManager::continueSearch";
    Q_ASSERT(!links_being_checked_);

    QList<LinkStatus*> const& node = nodeToAnalize();

    if(current_index_ < node.size())
        checkVectorLinks(node);

    else
    {
        current_index_ = 0;
        ++current_node_;

        kDebug(23100) <<  "Next node_____________________\n\n";
        
        if(current_node_ < (search_results_[current_depth_ - 1]).size() )
            checkVectorLinks(nodeToAnalize());
        else
        {
            kDebug(23100) <<  "Next Level_____________________________________________________________________________________\n\n\n";
            
            if(search_mode_ == SearchManager::domain ||
                    current_depth_ < depth_)
            {
                current_node_ = 0;
                ++current_depth_;

                emit signalAddingLevel(true);

//                 kDebug(23100) << "ThreadWeaver Queue length: " << m_weaver.queueLength();
                m_weaver.enqueue(new AddLevelJob(*this));
            }
            else
            {
                finnish();
            }
        }
    }
}

QList<LinkStatus*> const& SearchManager::nodeToAnalize() const
{
    Q_ASSERT(current_depth_ == search_results_.size());
    Q_ASSERT(current_node_ < (search_results_[current_depth_ - 1]).size());

    return (search_results_[current_depth_ - 1])[current_node_];
}

void SearchManager::checkVectorLinks(QList<LinkStatus*> const& links)
{
    checkLinksSimultaneously(chooseLinks(links), false);
}

void SearchManager::checkVectorLinksToRecheck(QList<LinkStatus*> const& links)
{
    checkLinksSimultaneously(chooseLinksToRecheck(links), true);
}

QList<LinkStatus*> SearchManager::chooseLinks(QList<LinkStatus*> const& links)
{
    if(current_index_ == 0) {
        kDebug(23100) << "Node parent: " << links[0]->parent()->absoluteUrl();
    }
  
    QList<LinkStatus*> escolha;
    for(int i = 0; i != max_simultaneous_connections_; ++i)
    {
        if(current_index_ < links.size())
            escolha.push_back(links[current_index_++]);
    }
    return escolha;
}

QList<LinkStatus*> SearchManager::chooseLinksToRecheck(QList<LinkStatus*> const& links)
{
    QList<LinkStatus*> sample;
    for(int i = 0; i != max_simultaneous_connections_; ++i)
    {
        if(recheck_current_index_ < links.size())
            sample.push_back(links[recheck_current_index_++]);
    }
    return sample;
}

void SearchManager::checkLinksSimultaneously(QList<LinkStatus*> const& links, bool recheck)
{
    Q_ASSERT(finished_connections_ <= max_simultaneous_connections_);
    finished_connections_ = 0;
    links_being_checked_ = 0;
    maximum_current_connections_ = -1;

    if(links.size() < max_simultaneous_connections_)
        maximum_current_connections_ = links.size();
    else
        maximum_current_connections_ = max_simultaneous_connections_;

    for(int i = 0; i != links.size(); ++i)
    {
        LinkStatus* ls(links[i]);
        checkLink(ls, recheck);
    }
}

void SearchManager::checkLink(LinkStatus* ls, bool recheck)
{
    Q_ASSERT(ls);

    QString protocol = ls->absoluteUrl().protocol();

    ++links_being_checked_;
    Q_ASSERT(links_being_checked_ <= max_simultaneous_connections_);

    if(ls->malformed())
    {
        Q_ASSERT(ls->errorOccurred());
        Q_ASSERT(ls->status() == LinkStatus::MALFORMED);

        ls->setChecked(true);
        recheck ? slotLinkRechecked(ls, 0) : slotLinkChecked(ls, 0);
    }

    else if(ls->absoluteUrl().prettyUrl().contains("javascript:", Qt::CaseInsensitive))
    {
        ++ignored_links_;
        ls->setIgnored(true);
        ls->setErrorOccurred(true);
        ls->setError("Javascript not supported");
        ls->setStatus(LinkStatus::NOT_SUPPORTED);
        ls->setChecked(true);
        recheck ? slotLinkRechecked(ls, 0) : slotLinkChecked(ls, 0);
    }
    else
    {
        LinkChecker* checker = new LinkChecker(ls, time_out_, this);
        checker->setSearchManager(this);

        if(recheck) {
            connect(checker, SIGNAL(transactionFinished(LinkStatus*, LinkChecker*)),
                    this, SLOT(slotLinkRechecked(LinkStatus*, LinkChecker*)));
        }
        else {
            connect(checker, SIGNAL(transactionFinished(LinkStatus*, LinkChecker*)),
                    this, SLOT(slotLinkChecked(LinkStatus*, LinkChecker*)));
        }
        checker->check();
    }
}

void SearchManager::recheckLink(LinkStatus* ls)
{
    checkLink(ls, true);
}

void SearchManager::linkRedirectionChecked(LinkStatus* link, bool recheck)
{
    kDebug(23100) <<  "SearchManager::linkRedirectionChecked: " << checked_links_;
    
    emit signalRedirection();
    recheck ? emit signalLinkRechecked(link) : emit signalLinkChecked(link);
  
    if(!recheck) {
        ++checked_links_;
        search_results_hash_.insert(link->absoluteUrl(), link);
    } else {
        ++links_rechecked_;
    }
    
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection(), recheck);
}

void SearchManager::slotLinkChecked(LinkStatus* link, LinkChecker* checker)
{
    kDebug(23100) <<  "SearchManager::slotLinkChecked: " << checked_links_;
//     kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
//             LinkStatus::lastRedirection((const_cast<LinkStatus*> (link)))->absoluteUrl().url() << endl;

    checker->deleteLater();

    Q_ASSERT(link);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
      LinkStatusHelper::validateMarkup(link);

    emit signalLinkChecked(link);
    
    if(link->isRedirection() && link->redirection())
        linkRedirectionChecked(link->redirection());
    
    ++checked_links_;
    ++finished_connections_;
    --links_being_checked_;
  
    Q_ASSERT(links_being_checked_ >= 0);

    if(search_mode_ != depth || current_depth_ < depth_) {
        m_weaver.enqueue(new BuildNodeJob(*this, link));
    }

    if(canceled_ && searching_ && !links_being_checked_)
    {
        pause();
    }

    else if(!canceled_ && finished_connections_ == maximumCurrentConnections() )
    {
        continueSearch();
        return;
    }
}

void SearchManager::slotLinkRechecked(LinkStatus* link, LinkChecker* checker)
{
    kDebug(23100) <<  "SearchManager::slotLinkRechecked";
//     kDebug(23100) <<  link->absoluteUrl().url() << " -> " << 
//             LinkStatus::lastRedirection((const_cast<LinkStatus*> (link)))->absoluteUrl().url() << endl;

    checker->deleteLater();

    ++links_rechecked_;
    ++finished_connections_;
    --links_being_checked_;
    
    Q_ASSERT(link);

    if(KLSConfig::showMarkupStatus() && link->isHtmlDocument())
        LinkStatusHelper::validateMarkup(link);

    emit signalLinkRechecked(link);

    // Nope, ignore redirections as they were already passed to the links to recheck
//     if(link->isRedirection() && link->redirection())
//         linkRedirectionChecked(link->redirection(), true);

    Q_ASSERT(links_being_checked_ >= 0);

    if(canceled_ && searching_ && !links_being_checked_)
    {
        pause();
    }
    else if(!canceled_ && finished_connections_ == maximumCurrentConnections() )
    {
        if(recheck_current_index_ < recheck_links_.size())
            continueRecheck();
        else
            finnish();
    }
}

void SearchManager::buildNewNode(LinkStatus* linkstatus)
{
    KUrl const& url = linkstatus->absoluteUrl();
    if(url.hasRef()) {
        KUrl urlWithoutRef = url;
        urlWithoutRef.setRef(QString());
        if(search_results_hash_.contains(url))
            return;
    }
    
//     kDebug(23100) << "SearchManager::buildNewNode";

    QList<LinkStatus*> new_node;
    fillWithChildren(linkstatus, new_node);
    
    if(new_node.size() == 0)
        return;
        
    // Push node
    m_mutex.lock();
    new_level_.push_back(new_node);
    m_mutex.unlock();

    emit signalNewLinksToCheck(new_node.size());
}

void SearchManager::addLevel()
{
    kDebug(23100) << "SearchManager::addLevel";

    if(new_level_.size() != 0) {
        m_mutex.lock();
        search_results_.push_back(new_level_);
        new_level_.clear();
        m_mutex.unlock();
    }
    
    emit signalAddingLevel(false);
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

    //kDebug(23100) <<  "url " << url.url() << " is checkable!";
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
            kDebug(23100) <<  "\n\nURL " << url.url() << " is not checkable by domain\n\n";
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
            kDebug(23100) <<  "Domain nao vago";
            return false;
        }
        else
        {
            QList<QString> palavras = tokenizeWordsSeparatedByDots(domain_);
            Q_ASSERT(palavras.size() >= 1); // host might be localhost

            QString primeira_palavra = palavras[0];
            if(primeira_palavra == "www")
            {
                Q_ASSERT(palavras.size() >= 3);
                kDebug(23100) <<  "Domain vago";
                return true;
            }
            else if(palavras.size() == 2)
            {
                kDebug(23100) <<  "Domain vago";
                return true;
            }
            else
            {
                kDebug(23100) <<  "Domain nao vago";
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

KHTMLPart* SearchManager::htmlPart(QString const& key_url) const
{
    return html_parts_.value(key_url, 0);
}

void SearchManager::addHtmlPart(QString const& key_url, KHTMLPart* html_part)
{
    Q_ASSERT(!key_url.isEmpty());
    Q_ASSERT(html_part);

    // FIXME configurable
//     if(html_parts_.size() > 300)
//         removeHtmlParts();

    html_parts_.insert(key_url, html_part);
}

void SearchManager::removeHtmlParts()
{
    for(KHTMLPartMap::iterator it = html_parts_.begin(); it != html_parts_.end(); ++it)
    {
        delete it.value();
        it.value() = 0;
    }

    html_parts_.clear();
}

void SearchManager::save(QDomElement& element, LinkStatusHelper::Status status) const
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
    
    for(int i = 0; i != search_results_.size(); ++i)
    {
        for(int j = 0; j != search_results_[i].size() ; ++j)
        {
            for(int l = 0; l != (search_results_[i])[j].size(); ++l)
            {
                LinkStatus* ls = ((search_results_[i])[j])[l];
                if(ls->checked() && LinkStatusHelper::hasStatus(ls, status))
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
void SearchManager::slotJobDone(ThreadWeaver::Job* job)
{
    AddLevelJob* addLevelJob = dynamic_cast<AddLevelJob*> (job);
    if(addLevelJob) {
        slotLevelAdded();
        return;
    }
    
//     kDebug(23100) << "Job not handled in SearchManager::slotJobDone: " << job;
}

QStringList SearchManager::findUnreferredDocuments(KUrl const& baseDir, QStringList const& documentList) const
{
    m_mutex.lock();
    // The hash can be modified elsewhere, so make a copy
    // (which is very fast because QList is implicitely shared
    QHash<KUrl, LinkStatus*> search_results_hash(search_results_hash_);
    m_mutex.unlock();
    
    Q_ASSERT(search_results_hash.size() != 0);

    QStringList unreferredDocuments;
    
    for(int i = 0; i != documentList.size(); ++i)
    {
        QString document = documentList[i];

//         kDebug(23100) << "Document: " << document;
        
        KUrl documentUrl(baseDir);    
        documentUrl.addPath(document);

        bool found = false;

        QHash<KUrl, LinkStatus*>::const_iterator it = search_results_hash.constBegin();
        for(it = search_results_hash.constBegin(); it != search_results_hash.constEnd(); ++it)
        {
            KUrl const& url(it.key());
            
            if(url == documentUrl) {
                found = true;
                break;
            }
        }
        if(!found) {
            unreferredDocuments.append(document);
            emit signalUnreferredDocFound(document);
        }

        emit signalUnreferredDocStepCompleted();
    }

    return unreferredDocuments;
}

QList<LinkStatus*> SearchManager::getLinksWithHtmlProblems() const
{
    m_mutex.lock();
    // The hash can be modified elsewhere, so make a copy
    // (which is very fast because QList is implicitely shared
    QHash<KUrl, LinkStatus*> search_results_hash(search_results_hash_);
    m_mutex.unlock();
    
    Q_ASSERT(search_results_hash.size() != 0);

    QList<LinkStatus*> links;
    QHash<KUrl, LinkStatus*>::const_iterator it = search_results_hash.constBegin();
    for(it = search_results_hash.constBegin(); it != search_results_hash.constEnd(); ++it)
    {
        LinkStatus* ls = it.value();
        if(ls->hasHtmlProblems())
            links.push_back(ls);
    }

    return links;
}


// Jobs

// BuildNodeJob

BuildNodeJob::BuildNodeJob(SearchManager& manager, LinkStatus* linkstatus)
  : m_searchManager(manager), m_linkStatus(linkstatus)
{
}

BuildNodeJob::~BuildNodeJob()
{
}
    
void BuildNodeJob::run()
{
//     kDebug(23100) << "\n\n\nBuildNodeJob::run\n\n\n";
    m_searchManager.buildNewNode(m_linkStatus);
}


// AddLevelJob

AddLevelJob::AddLevelJob(SearchManager& manager)
  : m_searchManager(manager)
{
}

AddLevelJob::~AddLevelJob()
{
}
    
void AddLevelJob::run()
{
//     kDebug(23100) << "\n\n\nAddLevelJob::run\n\n\n";
    while(m_searchManager.m_weaver.queueLength() != 0) {
        kDebug(23100) << "AddLevelJob::run: waiting for running jobs to finish";
        sleep(1);
    }
    m_searchManager.addLevel();
}

#include "searchmanager.moc"
