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

#include "linkstatus.h"
#include "../parser/node.h"



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
    assert(!node_);
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
    assert(!parent_);
    base_URI_ = "";
}

QString const LinkStatus::toString() const
{
    QString aux;

    if(!is_root_)
    {
        assert(parent_);
        aux += ("Parent: " + parent()->absoluteUrl().prettyURL() + "\n");
    }
    assert(!original_url_.isNull());

    aux += ("URL: " + absoluteUrl().prettyURL() + "\n");
    aux += ("Original URL: " + originalUrl() + "\n");
    if(node())
        aux += ("Node: " + node()->content() + "\n");

    return aux;
}


LinkStatus* LinkStatus::lastRedirection(LinkStatus* ls)
{
    if(ls->isRedirection())
        if(ls->redirection() != NULL)
            return lastRedirection(ls->redirection());
        else
            return ls;
    else
        return ls;
}

void LinkStatus::loadNode()
{
    assert(node_);

    setOriginalUrl(node_->url());
    setLabel(node_->linkLabel());

    if(malformed())
    {
        setErrorOccurred(true);
        setError("Malformed");
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
        setError("Malformed");
        kdDebug(23100) <<  "Malformed!" << endl;
        kdDebug(23100) <<  node()->content() << endl;
        //kdDebug(23100) <<  toString() << endl; // probable segfault
    }
    else if(error() == "Malformed")
    {
        setErrorOccurred(false);
        setError("");
    }
}

