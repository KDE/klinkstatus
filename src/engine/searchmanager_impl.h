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

#ifndef SEARCHMANAGER_IMPL_H
#define SEARCHMANAGER_IMPL_H

    
inline int SearchCounters::totalLinks() const {
    return total_links_;
}

inline int SearchCounters::brokenLinks() const {
    return broken_links_;
}

inline int SearchCounters::undeterminedLinks() const {
    return undetermined_links_;
}

inline SearchCounters const& SearchManager::searchCounters() const
{
    return search_counters_;
}

inline int SearchManager::maximumCurrentConnections() const
{
    Q_ASSERT(maximum_current_connections_ != -1);
    return maximum_current_connections_;
}

inline SearchManager::SearchMode const& SearchManager::searchMode() const
{
    return search_mode_;
}

inline int SearchManager::checkedLinks() const
{
    Q_ASSERT(search_counters_.total_links_ > 0);
    return search_counters_.total_links_;
}



inline void SearchManager::startSearch()
{
    Q_ASSERT(root_url_.isValid());
    startSearch(root_url_);
}

inline void SearchManager::startSearch(KUrl const& root)
{
    startSearch(root, search_mode_);
}

inline void SearchManager::setIsLoginPostRequest(bool is)
{
    is_login_post_request_ = is;
}

inline void SearchManager::setPostUrl(QString const& url)
{
    post_url_ = url;
}

inline void SearchManager::setPostData(QByteArray const& data)
{
    post_data_ = data;
}

inline void SearchManager::setSearchMode(SearchMode modo)
{
    search_mode_ = modo;
}

inline void SearchManager::setDepth(int depth)
{
    depth_ = depth;
}

inline void SearchManager::setExternalDomainDepth(int depth)
{
    external_domain_depth_ = depth;
}

inline void SearchManager::setDomain(QString const& domain)
{
    Q_ASSERT(domain.indexOf("http://") == -1);
    domain_ = domain;
    general_domain_ = generalDomain();
    checked_general_domain_ = true;
}

inline void SearchManager::setCheckParentDirs(bool flag)
{
    check_parent_dirs_ = flag;
}

inline void SearchManager::setCheckExternalLinks(bool flag)
{
    check_external_links_ = flag;
    external_domain_depth_ = check_external_links_ ? 1 : 0;
}

inline void SearchManager::setCheckRegularExpressions(bool flag)
{
    check_regular_expressions_ = flag;
}

inline void SearchManager::setRegularExpression(QString const& reg_exp, bool case_sensitive)
{
//     int case = case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    reg_exp_ = QRegExp(reg_exp, case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
}

inline void SearchManager::setTimeOut(int time_out)
{
    Q_ASSERT(time_out > 0);
    time_out_ = time_out;
}

inline void SearchManager::setRootUrl(KUrl const& url)
{
    root_url_ = url;
}




inline bool SearchManager::checkParentDirs() const
{
    return check_parent_dirs_;
}

inline bool SearchManager::checkExternalLinks() const
{
    return check_external_links_;
}

inline LinkStatus const* SearchManager::linkStatusRoot() const
{
    return &root_;
}

inline bool SearchManager::searching() const
{
    return searching_;
}

inline bool SearchManager::localDomain(KUrl const& url, bool restrict) const
{
	return Url::localDomain(root_.absoluteUrl(), url, restrict);
}

inline int SearchManager::maxSimultaneousConnections() const
{
    return max_simultaneous_connections_;
}

inline int SearchManager::timeOut() const
{
    return time_out_;
}

inline bool SearchManager::hasDocumentRoot() const
{
    return has_document_root_;
}

inline KUrl const& SearchManager::documentRoot() const
{
    return document_root_url_;
}

inline void SearchManager::setDocumentRoot(KUrl const& url)
{
    Q_ASSERT(url.isValid()); // includes empty URLs
    Q_ASSERT(!url.protocol().startsWith("http"));
    
    document_root_url_ = url;
    has_document_root_ = true;
}

inline ThreadWeaver::Weaver* SearchManager::threadWeaver() const
{
    return &m_weaver;
}


#endif // SEARCHMANAGER_IMPL_H
