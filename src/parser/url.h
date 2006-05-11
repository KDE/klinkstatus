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

#ifndef URL_H
#define URL_H

#include "../engine/linkstatus.h"
#include "node.h"

#include <kurl.h>
#include <QString>

#include <vector>

using namespace std;


class LinkStatus;

namespace Url
{
Node::LinkType resolveLinkType(QString const& url);
KUrl normalizeUrl(QString const& string_url, LinkStatus const& link_parent, QString const& document_root);
KUrl normalizeUrl(QString const& string_url);
bool validUrl(KUrl const& url);
bool existUrl(KUrl const& url, vector<LinkStatus*> const& v);
bool equalHost(QString const& host1, QString const& host2, bool restrict = false);
bool hasProtocol(QString const& url);
QString convertToLocal(LinkStatus const* ls);
bool localDomain(KUrl const& url1, KUrl const& url2, bool restrict = true);
bool parentDir(KUrl const& url1, KUrl const& url2);
bool externalLink(KUrl const& url1, KUrl const& url2, bool restrict = true);
}

inline bool validUrl(KUrl const& url)
{
  return (url.isValid() /*&& url.hasHost()*/);
}

#endif
