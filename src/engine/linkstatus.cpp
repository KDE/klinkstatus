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
    : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), error_(-1), 
        is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0), 
        tidy_info_()
{
}

LinkStatus::LinkStatus(KUrl const& absolute_url)
    : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), error_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0), 
        tidy_info_()
{
    setAbsoluteUrl(absolute_url);
}

LinkStatus::LinkStatus(Node* node, LinkStatus* parent)
    : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), error_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(node), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0), 
        tidy_info_()
{
    loadNode();

    setDepth(parent->depth() + 1);
    setParent(parent);
    setRootUrl(parent->rootUrl());
}

LinkStatus::~LinkStatus()
{
    for(int i = 0; i != children_nodes_.size(); ++i)
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

QString LinkStatus::statusText() const
{
    if(errorOccurred())
        return error();
    else if(!absoluteUrl().protocol().startsWith("http"))
        return status_text_;
    else
    {
//         int statusCode = httpHeader().statusCode();
//         kDebug(23100) << "Status Code: " << statusCode << endl;
//         kDebug(23100) << "Status Code Reason Phrase: " << httpHeader().reasonPhrase() << endl;

/*        QString const& reasonPhrase = httpHeader().reasonPhrase();
        if(reasonPhrase.isEmpty())
            return status_text_;*/
        
        QString string_code = QString::number(httpHeader().statusCode());
        if(absoluteUrl().hasRef()) // ref URL
            return status_text_;
        else if(string_code == "200"/* or string_code == "304"*/)
            return "OK";
        else
            return string_code;
    }
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
        kDebug(23100) <<  "Malformed:";
        kDebug(23100) <<  "Node: " << node()->content();
        //kDebug(23100) <<  toString(); // probable segfault
    }
}

bool LinkStatus::malformed() const // don't inline please (#include "node.h")
{
    return (malformed_ || (node_ && node_->malformed()));
}

// don't inline please (#include "node.h")
void LinkStatus::setChildrenNodes(QList<Node*> const& nodes) 
{
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
        kDebug(23100) <<  "Malformed!";
        kDebug(23100) <<  node()->content();
        //kDebug(23100) <<  toString(); // probable segfault
    }
    else if(error() == "Malformed")
    {
        setErrorOccurred(false);
        setError("");
        setStatus(LinkStatus::UNDETERMINED);
    }
}

bool LinkStatus::hasHtmlProblems() const
{
    return hasHtmlWarnings() || hasHtmlErrors();
}

bool LinkStatus::hasHtmlErrors() const
{
  if(!isHtmlDocument())
      return false;
    
  return tidy_info_.has_errors;
}

bool LinkStatus::hasHtmlWarnings() const
{
  if(!isHtmlDocument())
    return false;
    
  return tidy_info_.has_warnings;
}
