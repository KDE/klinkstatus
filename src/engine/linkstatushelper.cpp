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


void LinkStatusHelper::save(LinkStatus const* linkstatus, QDomElement& element)
{
    QDomElement child_element = element.ownerDocument().createElement("link");

    // <url>
    QDomElement tmp_1 = element.ownerDocument().createElement("url");
    tmp_1.appendChild(element.ownerDocument().createTextNode(linkstatus->absoluteUrl().prettyUrl()));
    child_element.appendChild(tmp_1);
    
    // <status>
    tmp_1 = element.ownerDocument().createElement("status");
    tmp_1.setAttribute("broken", isBroken(linkstatus) ? "true" : "false");
    tmp_1.appendChild(element.ownerDocument().createTextNode(linkstatus->statusText()));
    child_element.appendChild(tmp_1);

    // <label>
    tmp_1 = element.ownerDocument().createElement("label");
    tmp_1.appendChild(element.ownerDocument().createTextNode(KCharsets::resolveEntities(linkstatus->label())));
    child_element.appendChild(tmp_1);

    // <referrers>
    tmp_1 = element.ownerDocument().createElement("referrers");
    
    Q3ValueList<KUrl> referrers = linkstatus->referrers();
    for(Q3ValueList<KUrl>::const_iterator it = referrers.begin(); it != referrers.end(); ++it)
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

bool LinkStatusHelper::hasStatus(LinkStatus const* linkstatus, LinkStatusHelper::Status ui_status)
{
    LinkStatus::Status detailed_status = linkstatus->status();
    
    if(ui_status == good)
    {
        return (detailed_status == LinkStatus::HTTP_REDIRECTION
               || detailed_status == LinkStatus::SUCCESSFULL);
    }
    else if(ui_status == bad)
    {
    return
        (detailed_status == LinkStatus::BROKEN
        || detailed_status == LinkStatus::HTTP_CLIENT_ERROR
        || detailed_status == LinkStatus::HTTP_SERVER_ERROR
        || detailed_status == LinkStatus::MALFORMED);
    }
    else if(ui_status == malformed)
    {
        return (detailed_status == LinkStatus::MALFORMED);
    }
    else if(ui_status == undetermined)
    {
        return (detailed_status == LinkStatus::NOT_SUPPORTED
               || detailed_status == LinkStatus::TIMEOUT
               || detailed_status == LinkStatus::UNDETERMINED);
    }
    else
        return true;
}

bool LinkStatusHelper::isGood(LinkStatus const* linkstatus)
{
    return hasStatus(linkstatus, LinkStatusHelper::good);
}

bool LinkStatusHelper::isBroken(LinkStatus const* linkstatus)
{
    return hasStatus(linkstatus, LinkStatusHelper::bad);
}

bool LinkStatusHelper::isMalformed(LinkStatus const* linkstatus)
{
    return hasStatus(linkstatus, LinkStatusHelper::malformed);
}

bool LinkStatusHelper::isUndetermined(LinkStatus const* linkstatus)
{
    return hasStatus(linkstatus, LinkStatusHelper::undetermined);
}

QString const LinkStatusHelper::toString(LinkStatus const* linkstatus)
{
    QString aux;

    if(!linkstatus->isRoot())
    {
        Q_ASSERT(linkstatus->parent());
        aux += "Parent: " + linkstatus->parent()->absoluteUrl().prettyUrl() + '\n';
    }
    Q_ASSERT(!linkstatus->originalUrl().isNull());

    aux += "URL: " + linkstatus->absoluteUrl().prettyUrl() + '\n';
    aux += "Original URL: " + linkstatus->originalUrl() + '\n';
    if(linkstatus->node())
        aux += "Node: " + linkstatus->node()->content() + '\n';

    return aux;
}

void LinkStatusHelper::validateMarkup(LinkStatus const* linkstatus)
{
    Tidy::MarkupValidator markup_validator(linkstatus->absoluteUrl(), linkstatus->docHtml());
    markup_validator.validate();
    
    linkstatus->tidy_info_->has_errors = markup_validator.hasErrors();
    linkstatus->tidy_info_->has_warnings = markup_validator.hasWarnings();
//     tidy_messages_ = markup_validator.messages();
}
