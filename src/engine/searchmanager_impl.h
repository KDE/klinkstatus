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
    Q_ASSERT(checked_links_ > 0);
    return checked_links_;
}

inline QTime SearchManager::timeElapsed() const
{
    int ms = time_.elapsed();
    //kdDebug(23100) <<  "Time elapsed (ms): " << ms << endl;
    return QTime(0, 0).addMSecs(ms);
}

inline void SearchManager::startSearch(KURL const& root)
{
    startSearch(root, search_mode_);
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
    Q_ASSERT(domain.find("http://") == -1);
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
}

inline void SearchManager::setCheckRegularExpressions(bool flag)
{
    check_regular_expressions_ = flag;
}

inline void SearchManager::setRegularExpression(QString const& reg_exp, bool case_sensitive)
{
    reg_exp_ = QRegExp(reg_exp, case_sensitive);
}

inline void SearchManager::setTimeOut(int time_out)
{
    Q_ASSERT(time_out > 0);
    time_out_ = time_out;
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

inline bool SearchManager::localDomain(KURL const& url, bool restrict) const
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

inline KURL const& SearchManager::documentRoot() const
{
    return document_root_url_;
}

inline void SearchManager::setDocumentRoot(KURL const& url)
{
    Q_ASSERT(url.isValid()); // includes empty URLs
    Q_ASSERT(!url.protocol().startsWith("http"));
    
    document_root_url_ = url;
    has_document_root_ = true;
}


