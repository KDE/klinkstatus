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

#include <cassert>


inline int SearchManager::maximumCurrentConnections() const
{
  assert(maximum_current_connections_ != -1);
  return maximum_current_connections_;
}

inline SearchManager::SearchMode const& SearchManager::searchMode() const
{
  return search_mode_;
}

inline int SearchManager::checkedLinks() const
{
  assert(checked_links_ > 0);
  return checked_links_;
}

inline QTime SearchManager::timeElapsed() const
{
  int ms = time_.elapsed();
  //cerr << "Time elapsed (ms): " << ms << endl;
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
  assert(domain.find("http://") == -1);
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
	return ::localDomain(root_.absoluteUrl(), url, restrict);
}

inline int SearchManager::maxSimultaneousConnections() const
{
	return max_simultaneous_connections_;
}
	
inline int SearchManager::timeOut() const
{
	return time_out_;
}

