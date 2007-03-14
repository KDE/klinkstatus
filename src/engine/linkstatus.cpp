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

#include <kcharsets.h>

#include <QDomElement>

#include "../parser/node.h"
#include "../ui/treeview.h"

    
LinkStatus::LinkStatus()
    : depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0), 
        tidy_info_(new TidyInfo)
{
}

LinkStatus::LinkStatus(KUrl const& absolute_url)
    : depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0)
{
    setAbsoluteUrl(absolute_url);
}

LinkStatus::LinkStatus(Node* node, LinkStatus* parent)
    : depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(node), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0), 
        tidy_info_(new TidyInfo)
{
    loadNode();

    setDepth(parent->depth() + 1);
    setParent(parent);
    setRootUrl(parent->rootUrl());
}

LinkStatus::~LinkStatus()
{
    //kDebug(23100) <<  "|";

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

    delete tidy_info_;
    tidy_info_ = 0;
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

void LinkStatus::loadNode()
{
    Q_ASSERT(node_);

    setOriginalUrl(node_->url());
    setLabel(node_->linkLabel());

    if(malformed())
    {
        setErrorOccurred(true);
        setError("Malformed");
        setStatus(LinkStatus::MALFORMED);
        kDebug(23100) <<  "Malformed:" << endl;
        kDebug(23100) <<  "Node: " << node()->content() << endl;
        //kDebug(23100) <<  toString() << endl; // probable segfault
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
        setStatus(LinkStatus::MALFORMED);
        kDebug(23100) <<  "Malformed!" << endl;
        kDebug(23100) <<  node()->content() << endl;
        //kDebug(23100) <<  toString() << endl; // probable segfault
    }
    else if(error() == "Malformed")
    {
        setErrorOccurred(false);
        setError("");
        setStatus(LinkStatus::UNDETERMINED);
    }
}

bool LinkStatus::hasHtmlErrors() const
{
  if(!isHtmlDocument())
    return false;
    
  return tidy_info_->has_errors;
}

bool LinkStatus::hasHtmlWarnings() const
{
  if(!isHtmlDocument())
    return false;
    
  return tidy_info_->has_warnings;
}
