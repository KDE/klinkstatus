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

#include <kresolver.h>

#include "url.h"
#include "mstring.h"
#include "../utils/utils.h"

#include <kcharsets.h>


Node::LinkType Url::resolveLinkType(QString const& url)
{
    QString aux(url);
    aux = KURL::decode_string(aux);

    if(aux.isNull())
        return Node::relative;

    if(findWord(url, "FILE:") != -1)
        return Node::file_href;
    else if(findWord(KCharsets::resolveEntities(url), "MAILTO:") != -1)
        return Node::mailto;
    else if( (int)url.find(":/") != -1)
        return Node::href;
    else
        return Node::relative;
}

KURL Url::normalizeUrl(QString const& string_url, LinkStatus const& link_parent, QString const& document_root)
{
    QString _string_url = string_url.stripWhiteSpace();

    QString s_url;
    KURL base_url;

    // resolve base url
    if(link_parent.hasBaseURI())
        base_url = link_parent.baseURI();
    else
        base_url = link_parent.absoluteUrl();

    // resolve relative url
    if(_string_url.isEmpty())
        return base_url;
    else if(Url::hasProtocol(_string_url))
        return KURL(_string_url);
    else
    {
        s_url.prepend(base_url.protocol() + "://" + base_url.host());

        if(_string_url[0] == '/') {
            if(!base_url.protocol().startsWith("http")) {
                s_url.append(document_root);
            }
        }
        else {
            s_url.append(base_url.directory(true, false) + "/");                
        }

        if( (_string_url[0] == ';' || // parameters
                _string_url[0] == '?' || // query
                _string_url[0] == '#') ) // fragment or reference
        {
            s_url.append(base_url.fileName(false));
        }

        s_url.append(_string_url);
        KURL url(s_url);
        if(base_url.hasUser())
            url.setUser(base_url.user());
        if(base_url.hasPass())
            url.setPass(base_url.pass());

        url.setPort(base_url.port());

        url.cleanPath();

//         kdDebug(23100) << "Normalized URL: " 
//                 << KCharsets::resolveEntities(KURL::decode_string(url.url())) << endl;

        return KURL(KCharsets::resolveEntities(KURL::decode_string(url.url())));
    }
}

KURL Url::normalizeUrl(QString const& string_url)
{
    QString qs_url(KCharsets::resolveEntities(string_url.stripWhiteSpace()));

    if(qs_url[0] == '/')
    {
        KURL url;
        url.setPath(qs_url);
        url.cleanPath();
        return url;
    }

    else
    {
        if(!Url::hasProtocol(qs_url))
            qs_url.prepend("http://");

        KURL url(qs_url);
        url.cleanPath();
        return url;
    }
}

bool Url::existUrl(KURL const& url, vector<LinkStatus*> const& v)
{
    if(url.prettyURL().isEmpty())
        return true;

    for(uint i = 0; i != v.size(); ++i)
        if(v[i]->absoluteUrl() == url)
            return true;

    return false;
}

/**
   www.iscte.pt, iscte.pt => true;
   iscte.pt, www.iscte.pt => true;
   www.iscte.pt, alunos.iscte.pt => true; (if restrict = false)
   www.iscte.pt, alunos.iscte.pt => false; (if restrict = true)
   alunos.iscte.pt, www.iscte.pt => false;
   alunos.iscte.pt, iscte.pt => false.
*/
// FIXME - Rename this function to sameDomain
bool Url::equalHost(QString const& host1, QString const& host2, bool restrict)
{
    //Q_ASSERT(!host1.isEmpty());
    //Q_ASSERT(!host2.isEmpty()); // this fails if href="javascript:......."
    //if(host2.isEmpty())
    //return false;

    if(host1 == host2)
        return true;

    QString host1_(KNetwork::KResolver::normalizeDomain(host1));
    QString host2_(KNetwork::KResolver::normalizeDomain(host2));
    removeLastCharIfExists(host1_, '/');
    removeLastCharIfExists(host2_, '/');

    vector<QString> v1 = tokenizeWordsSeparatedByDots(host1_);
    vector<QString> v2 = tokenizeWordsSeparatedByDots(host2_);
    uint const size1 = v1.size();
    uint const size2 = v2.size();

    if( !(size1 >= 1 && size2 >= 1) && // localhost would have size = 1
            !(host1_[0].isNumber() || host2_[0].isNumber()) ) // not (host == IP)
    {
        kdDebug(23100) <<  "Invalid host: " << host2 << endl;
        return false;
    }

    vector<QString>::size_type aux = 0;
    vector<QString>::size_type aux2 = 0;
    if(v1[0] == "www")
        aux = 1;
    if(v2[0] == "www")
        aux2 = 1;

    if((size2 - aux2 < size1 - aux) && restrict) // e.g. paradigma.co.pt < linkstatus.paradigma.co.pt
        return false;

    if(restrict && (size2 - aux2 > size1 - aux)) // e.g. linkstatus.paradigma.co.pt > paradigma.co.pt
        return false;

    int i = 1;
    while( ((int)(size1 - i) >= (int)aux) && ((int)(size2 - i) >= (int)aux) )
    {
        if( !(v1[size1 - i] == v2[size2 - i]) )
            return false;

        ++i;
    }

    return true;
}

/* This should be done by parsing but I wan't to know when some new scheme comes along :) */
bool Url::hasProtocol(QString const& url)
{
    QString s_url(url);
    s_url.stripWhiteSpace();

    if(s_url[0] == '/')
        return false;

    else
    {
        KURL url = KURL::fromPathOrURL(s_url);
        if(!url.protocol().isEmpty())
            return true;
        /*
        if(s_url.startsWith("http:") ||
                s_url.startsWith("https:") ||
                s_url.startsWith("ftp:") ||
                s_url.startsWith("sftp:") ||
                s_url.startsWith("webdav:") ||
                s_url.startsWith("webdavs:") ||
                s_url.startsWith("finger:") ||
                s_url.startsWith("fish:") ||
                s_url.startsWith("imap:") ||
                s_url.startsWith("imaps:") ||
                s_url.startsWith("lan:") ||
                s_url.startsWith("ldap:") ||
                s_url.startsWith("pop3:") ||
                s_url.startsWith("pop3s:") ||
                s_url.startsWith("smtp:") ||
                s_url.startsWith("smtps:") ||
                s_url.startsWith("file:") ||
                s_url.startsWith("news:") ||
                s_url.startsWith("gopher:") ||
                s_url.startsWith("mailto:") ||
                s_url.startsWith("telnet:") ||
                s_url.startsWith("prospero:") ||
                s_url.startsWith("wais:") ||
                s_url.startsWith("nntp:") )
        {
            return true;
        }
        */
        else
            return false;
    }
}

/**
	http://linkstatus.paradigma.co.pt/en/index.html&bix=bix -> /en/index.html&bix=bix
*/
QString Url::convertToLocal(LinkStatus const* ls)
{
    KURL url = ls->absoluteUrl();
    KURL base_url = ls->rootUrl();

    if(base_url == url)
        return "./" + url.fileName();
    else
        return KURL::relativeURL(base_url, url);
}

/**
	If url2 has the same domain has url1 returns true.
	If restrict, sourceforge.net != quanta.sourceforge.net.
	Else is equal.
*/
bool Url::localDomain(KURL const& url1, KURL const& url2, bool restrict)
{
    if(url1.protocol() != url2.protocol())
    {
        //kdDebug(23100) <<  "NOT localDomain" << endl;
        return false;
    }
    else if(!url1.hasHost())
    {
        //kdDebug(23100) <<  "localDomain" << endl;
        return true;
    }
    else
    {
        //return ::equalHost(url1.host(), url2.host(), restrict);
        if(Url::equalHost(url1.host(), url2.host(), restrict))
        {
            //kdDebug(23100) <<  "localDomain" << endl;
            return true;
        }
        else
        {
            //kdDebug(23100) <<  "NOT localDomain" << endl;
            return false;
        }

    }
}

/**
	Returns true if url2 is a parent of url1.
*/
bool Url::parentDir(KURL const& url1, KURL const& url2)
{
    if(url1.protocol() != url2.protocol())
        return false;

    else if(!url1.hasHost())
        return url2.isParentOf(url1);

    else
    {
        if(!equalHost(url1.host(), url2.host()))
            return false;
		
        vector<QString> tokens_1 = tokenizeWordsSeparatedBy(url1.directory(true, false), QChar('/'));
        vector<QString> tokens_2 = tokenizeWordsSeparatedBy(url2.directory(true, false), QChar('/'));
		
		if(tokens_1.size() == 0)
			return false;

        //if(tokens_2.size() > tokens_1.size() or tokens_2.size() == 0)
            //return true;
		vector<QString>::size_type size = 0;
		if(tokens_1.size() < tokens_2.size())
			size = tokens_1.size();
		else
			size = tokens_2.size();

        for(vector<QString>::size_type i = 0; i != size; ++i)
        {
            if(tokens_2[i] != tokens_1[i])
                return true;
        }
    }

    return false;
}

bool Url::externalLink(KURL const& url1, KURL const& url2, bool restrict)
{
    if(url1.protocol() != url2.protocol())
    {
        kdDebug(23100) <<  "externalLink" << endl;
        return true;
    }
    else if(!url1.hasHost() && !url2.hasHost())
    {
        kdDebug(23100) <<  "NOT externalLink" << endl;
        return false;
    }
    else
        return !Url::equalHost(url1.host(), url2.host(), restrict);
}
