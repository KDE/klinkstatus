/***************************************************************************
 *   Copyright (C) 2006 by Paulo Moura Guedes                              *
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

#include "linkstatushelper.h"

#include <kcharsets.h>

#include <qdom.h>

#include "linkstatus.h"


LinkStatusHelper::LinkStatusHelper(LinkStatus const* linkstatus)
    : linkstatus_(linkstatus)
{
    Q_ASSERT(linkstatus_);
}


LinkStatusHelper::~LinkStatusHelper()
{
}


void LinkStatusHelper::save(QDomElement& element) const
{
    QDomElement child_element = element.ownerDocument().createElement("link");

    // <url>
    QDomElement tmp_1 = element.ownerDocument().createElement("url");
    tmp_1.appendChild(element.ownerDocument().createTextNode(linkstatus_->absoluteUrl().prettyUrl()));
    child_element.appendChild(tmp_1);
    
    // <status>
    tmp_1 = element.ownerDocument().createElement("status");
    tmp_1.setAttribute("broken", isBroken() ? "true" : "false");
    tmp_1.appendChild(element.ownerDocument().createTextNode(linkstatus_->statusText()));
    child_element.appendChild(tmp_1);

    // <label>
    tmp_1 = element.ownerDocument().createElement("label");
    tmp_1.appendChild(element.ownerDocument().createTextNode(KCharsets::resolveEntities(linkstatus_->label())));
    child_element.appendChild(tmp_1);

    // <referers>
    tmp_1 = element.ownerDocument().createElement("referrers");
    
    Q3ValueVector<KUrl> referrers = linkstatus_->referrers();
    for(Q3ValueVector<KUrl>::const_iterator it = referrers.begin(); it != referrers.end(); ++it)
    {
        QDomElement tmp_2 = element.ownerDocument().createElement("url");
        tmp_2.appendChild(element.ownerDocument().createTextNode(it->prettyUrl()));
    
        tmp_1.appendChild(tmp_2);
    }
    Q_ASSERT(!referrers.isEmpty());
    child_element.appendChild(tmp_1);

    element.appendChild(child_element);
}

LinkStatus* LinkStatusHelper::lastRedirection(LinkStatus* ls)
{
    if(ls->isRedirection())
        if(ls->redirection())
            return lastRedirection(ls->redirection());
    
    return ls;
}

bool LinkStatusHelper::hasStatus(Status status) const
{
    if(status == good)
    {
        if(linkstatus_->errorOccurred())
            return false;
        else
            if(linkstatus_->absoluteUrl().protocol() != "http" &&
               linkstatus_->absoluteUrl().protocol() != "https")
                return (linkstatus_->statusText() == "OK" ||
                        (!linkstatus_->absoluteUrl().hasRef()));
        else
        {
            QString status_code(QString::number(linkstatus_->httpHeader().statusCode()));
            return (linkstatus_->statusText() == "OK" ||
                    (!linkstatus_->absoluteUrl().hasRef() &&
                    status_code[0] != '5' &&
                    status_code[0] != '4'));
        }
    }
    else if(status == bad)
    {
        return (!hasStatus(good) && !linkstatus_->error().contains("Timeout"));
    }
    else if(status == malformed)
    {
        return (linkstatus_->error() == "Malformed");
    }
    else if(status == undetermined)
    {
        return (linkstatus_->error().contains("Timeout") ||
                (linkstatus_->absoluteUrl().hasRef() && linkstatus_->statusText() != "OK"));
    }
    else
        return true;
}

bool LinkStatusHelper::isGood() const
{
    return hasStatus(good);
}

bool LinkStatusHelper::isBroken() const
{
    return hasStatus(bad);    
}

bool LinkStatusHelper::isMalformed() const
{
    return hasStatus(malformed);
}

bool LinkStatusHelper::isUndetermined() const
{
    return hasStatus(undetermined);
}

QString const LinkStatusHelper::toString() const
{
    QString aux;

    if(!linkstatus_->isRoot())
    {
        Q_ASSERT(linkstatus_->parent());
        aux += "Parent: " + linkstatus_->parent()->absoluteUrl().prettyUrl() + '\n';
    }
    Q_ASSERT(!linkstatus_->originalUrl().isNull());

    aux += "URL: " + linkstatus_->absoluteUrl().prettyUrl() + '\n';
    aux += "Original URL: " + linkstatus_->originalUrl() + '\n';
    if(linkstatus_->node())
        aux += "Node: " + linkstatus_->node()->content() + '\n';

    return aux;
}
