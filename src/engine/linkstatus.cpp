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

#include "linkstatus.h"
#include "../parser/node.h"
#include "../ui/treeview.h"

#include <klocale.h>
#include <kcharsets.h>

#include <qdom.h>


LinkStatus::~LinkStatus()
{
    //kdDebug(23100) <<  "|";

    for(uint i = 0; i != children_nodes_.size(); ++i)
    {
        if(children_nodes_[i])
        {
            delete children_nodes_[i];
            children_nodes_[i] = 0;
        }
    }

    children_nodes_.clear();

    if(isRedirection())
    {
        if(redirection_)
        {
            delete redirection_;
            redirection_ = 0;
        }
    }
}

void LinkStatus::reset()
{
    depth_ = -1;
    external_domain_depth_ = -1;
    is_root_ = false;
    error_occurred_ = false;
    is_redirection_ = false;
    checked_ = false;
    only_check_header_ = true;
    malformed_ = false;
    Q_ASSERT(!node_);
    has_base_URI_ = false;
    label_ = "";
    absolute_url_ = "";
    doc_html_ = "";
    http_header_ = HttpResponseHeader();
    error_ = "";

    for(uint i = 0; i != children_nodes_.size(); ++i)
    {
        if(children_nodes_[i])
        {
            delete children_nodes_[i];
            children_nodes_[i] = 0;
        }
    }

    children_nodes_.clear();

    if(isRedirection())
    {
        if(redirection_)
        {
            delete redirection_;
            redirection_ = 0;
        }
    }
    Q_ASSERT(!parent_);
    base_URI_ = "";
}

QString const LinkStatus::toString() const
{
    QString aux;

    if(!is_root_)
    {
        Q_ASSERT(parent_);
        aux += i18n( "Parent: %1" ).arg( parent()->absoluteUrl().prettyURL() ) + "\n";
    }
    Q_ASSERT(!original_url_.isNull());

    aux += i18n( "URL: %1" ).arg( absoluteUrl().prettyURL() ) + "\n";
    aux += i18n( "Original URL: %1" ).arg( originalUrl() ) + "\n";
    if(node())
        aux += i18n( "Node: %1" ).arg( node()->content() ) + "\n";

    return aux;
}


LinkStatus* LinkStatus::lastRedirection(LinkStatus* ls)
{
    if(ls->isRedirection())
        if(ls->redirection())
            return lastRedirection(ls->redirection());
        else
            return ls;
    else
        return ls;
}

void LinkStatus::loadNode()
{
    Q_ASSERT(node_);

    setOriginalUrl(node_->url());
    setLabel(node_->linkLabel());

    if(malformed())
    {
        setErrorOccurred(true);
        setError(i18n( "Malformed" ));
        setStatus(LinkStatus::MALFORMED);
        kdDebug(23100) <<  "Malformed:" << endl;
        kdDebug(23100) <<  "Node: " << node()->content() << endl;
        //kdDebug(23100) <<  toString() << endl; // probable segfault
    }
}

bool LinkStatus::malformed() const // don't inline please (#include "node.h")
{
    return (malformed_ || node_->malformed());
}

void LinkStatus::setChildrenNodes(vector<Node*> const& nodes) // don't inline please (#include "node.h")
{
    children_nodes_.reserve(nodes.size());
    children_nodes_ = nodes;
}

void LinkStatus::setMalformed(bool flag)
{
    malformed_ = flag;
    if(flag)
    {
        setErrorOccurred(true);
        setError(i18n( "Malformed" ));
        setStatus(LinkStatus::MALFORMED);
        kdDebug(23100) <<  "Malformed!" << endl;
        kdDebug(23100) <<  node()->content() << endl;
        //kdDebug(23100) <<  toString() << endl; // probable segfault
    }
    else if(error() == i18n( "Malformed" ))
    {
        setErrorOccurred(false);
        setError("");
        setStatus(LinkStatus::UNDETERMINED);
    }
}

void LinkStatus::save(QDomElement& element) const
{
    QDomElement child_element = element.ownerDocument().createElement("link");

    // <url>
    QDomElement tmp_1 = element.ownerDocument().createElement("url");
    tmp_1.appendChild(element.ownerDocument().createTextNode(absoluteUrl().prettyURL()));
    child_element.appendChild(tmp_1);
    
    // <status>
    tmp_1 = element.ownerDocument().createElement("status");
    tmp_1.setAttribute("broken", 
                       ResultView::displayableWithStatus(this, ResultView::bad) ? 
                               "true" : "false");
    tmp_1.appendChild(element.ownerDocument().createTextNode(statusText()));
    child_element.appendChild(tmp_1);

    // <label>
    tmp_1 = element.ownerDocument().createElement("label");
    tmp_1.appendChild(element.ownerDocument().createTextNode(KCharsets::resolveEntities(label())));
    child_element.appendChild(tmp_1);

    // <referers>
    tmp_1 = element.ownerDocument().createElement("referrers");
    
    for(QValueVector<KURL>::const_iterator it = referrers_.begin(); it != referrers_.end(); ++it)
    {
        QDomElement tmp_2 = element.ownerDocument().createElement("url");
        tmp_2.appendChild(element.ownerDocument().createTextNode(it->prettyURL()));
    
        tmp_1.appendChild(tmp_2);
    }
    Q_ASSERT(!referrers_.isEmpty());
    child_element.appendChild(tmp_1);

    element.appendChild(child_element);
}

