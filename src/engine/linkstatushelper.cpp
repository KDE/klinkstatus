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
#include <QSet>


void LinkStatusHelper::reset(LinkStatus* ls)
{
    ls->depth_ = -1;
    ls->external_domain_depth_ = -1;
    ls->is_root_ = false;
    ls->error_occurred_ = false;
    ls->is_redirection_ = false;
    ls->checked_ = false;
    ls->only_check_header_ = true;
    ls->malformed_ = false;
    Q_ASSERT(!ls->node_);
    ls->has_base_URI_ = false;
    ls->label_ = "";
    ls->absolute_url_ = "";
    ls->doc_html_ = "";
    ls->http_header_ = HttpResponseHeader();
    ls->error_ = "";
    ls->tree_view_item_ = 0;

    for(int i = 0; i != ls->children_nodes_.size(); ++i)
    {
        if(ls->children_nodes_[i])
        {
            delete ls->children_nodes_[i];
            ls->children_nodes_[i] = 0;
        }
    }

    ls->children_nodes_.clear();

    if(ls->isRedirection())
    {
        if(ls->redirection_)
        {
            delete ls->redirection_;
            ls->redirection_ = 0;
        }
    }
    Q_ASSERT(!ls->parent_);
    ls->base_URI_ = "";
}

/**
* Only reset the results not the initialization (absolute URL, etc)
*/
void LinkStatusHelper::resetResults(LinkStatus* ls)
{
    ls->error_occurred_ = false;
    ls->is_redirection_ = false;
    ls->checked_ = false;
    ls->only_check_header_ = true;
    ls->malformed_ = false;
    ls->http_header_ = HttpResponseHeader();
    ls->error_ = "";
}

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
    
    QSet<KUrl> referrers = linkstatus->referrers();
    foreach(KUrl url, referrers)
    {
        QDomElement tmp_2 = element.ownerDocument().createElement("url");
        tmp_2.appendChild(element.ownerDocument().createTextNode(url.prettyUrl()));
    
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

void LinkStatusHelper::validateMarkup(LinkStatus* linkstatus)
{
    Tidy::MarkupValidator markup_validator(linkstatus->absoluteUrl(), linkstatus->docHtml());
    markup_validator.validate();
    
    (linkstatus->tidy_info_).has_errors = markup_validator.hasErrors();
    (linkstatus->tidy_info_).has_warnings = markup_validator.hasWarnings();
//     tidy_messages_ = markup_validator.messages();
}
