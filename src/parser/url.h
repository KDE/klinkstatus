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

#ifndef URL_H
#define URL_H

#include "../engine/linkstatus.h"
#include "node.h"

#include <kurl.h>
#include <qstring.h>

#include <vector>

using namespace std;


class LinkStatus;

Node::LinkType resolveLinkType(QString const& url);
KURL normalizeUrl(QString const& string_url, LinkStatus const& link_parent);
KURL normalizeUrl(QString const& string_url);
bool validUrl(KURL const& url);
bool existUrl(KURL const& url, vector<LinkStatus*> const& v);
bool equalHost(QString const& host1, QString const& host2, bool restrict = false);
bool hasProtocol(QString const& url);
QString convertToLocal(LinkStatus const* ls);
bool localDomain(KURL const& url1, KURL const& url2, bool restrict = true);
bool parentDir(KURL const& url1, KURL const& url2);
bool externalLink(KURL const& url1, KURL const& url2, bool restrict = true);


inline bool validUrl(KURL const& url)
{
  return (url.isValid() /*&& url.hasHost()*/);
}

#endif
