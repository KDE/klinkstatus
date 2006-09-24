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

inline LinkStatus::LinkStatus()
        : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0)
{}

inline LinkStatus::LinkStatus(KURL const& absolute_url)
        : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(0), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0)
{
    setAbsoluteUrl(absolute_url);
}

inline LinkStatus::LinkStatus(Node* node, LinkStatus* parent)
        : status_(LinkStatus::UNDETERMINED), depth_(-1), external_domain_depth_(-1), is_root_(false),
        error_occurred_(false), is_redirection_(false), parent_(0), redirection_(0), checked_(false),
        only_check_header_(true), malformed_(false),
        node_(node), has_base_URI_(false), has_html_doc_title_(false), ignored_(false),
        mimetype_(""), is_error_page_(false), tree_view_item_(0)
{
    loadNode();

    setDepth(parent->depth() + 1);
    setParent(parent);
    setRootUrl(parent->rootUrl());
}

inline void LinkStatus::setRootUrl(KURL const& url)
{
    root_url_ = url;
}

inline void LinkStatus::setStatus(Status status)
{
    status_ = status;
}

inline void LinkStatus::setDepth(uint depth)
{
    depth_ = depth;
}

inline void LinkStatus::setParent(LinkStatus* parent)
{
    Q_ASSERT(parent);

    parent_ = parent;
    addReferrer(parent->absoluteUrl());
}

inline void LinkStatus::setAbsoluteUrl(KURL const& url_absoluto)
{
    absolute_url_ = url_absoluto;
}

inline void LinkStatus::setOriginalUrl(QString const& url_original)
{
    original_url_ = url_original;
}

inline void LinkStatus::setLabel(QString const& label)
{
    label_ = label;
}

inline void LinkStatus::setDocHtml(QString const& doc_html)
{
    Q_ASSERT(!doc_html.isEmpty());
    doc_html_ = doc_html;
}

inline void LinkStatus::setHttpHeader(HttpResponseHeader const& cabecalho_http)
{
    http_header_ = cabecalho_http;
}

inline void LinkStatus::setStatusText(QString const& status)
{
    Q_ASSERT(!status.isEmpty());
    status_text_ = status;
}

inline void LinkStatus::setError(QString const& error)
{
    Q_ASSERT(!error.isEmpty());
    error_ = error;
}

inline void LinkStatus::setErrorOccurred(bool houve_error)
{
    error_occurred_ = houve_error;
}

inline void LinkStatus::setIsRoot(bool flag)
{
    is_root_ = flag;
    label_ = i18n("ROOT");
}

inline void LinkStatus::setRedirection(LinkStatus* redirection)
{
    Q_ASSERT(redirection != NULL);
    Q_ASSERT(isRedirection());
    redirection_ = redirection;
}

inline void LinkStatus::setIsRedirection(bool e_redirection)
{
    is_redirection_ = e_redirection;
}

inline void LinkStatus::addChildNode(Node* node)
{
    children_nodes_.push_back(node);
}

inline void LinkStatus::reserveMemoryForChildrenNodes(int n)
{
    Q_ASSERT(n > 0);
    children_nodes_.reserve(n);
}

inline void LinkStatus::setChecked(bool flag)
{
    checked_ = flag;
}

inline void LinkStatus::setExternalDomainDepth(int p)
{
    Q_ASSERT(p >= -1);
    external_domain_depth_ = p;
}

inline void LinkStatus::setOnlyCheckHeader(bool flag)
{
    only_check_header_= flag;
}

inline void LinkStatus::setHasBaseURI(bool flag)
{
    has_base_URI_ = flag;
}

inline void LinkStatus::setHasHtmlDocTitle(bool flag)
{
    has_html_doc_title_ = flag;
}

inline void LinkStatus::setBaseURI(KURL const& base_url)
{
    if(!base_url.isValid())
    {
        kdWarning(23100) <<  "base url not valid: " << endl
        << "parent: " << parent()->absoluteUrl().prettyURL() << endl
        << "url: " << absoluteUrl().prettyURL() << endl
        << "base url resolved: " << base_url.prettyURL() << endl;
    }

    Q_ASSERT(base_url.isValid());
    has_base_URI_ = true;
    base_URI_ = base_url;
}

inline void LinkStatus::setHtmlDocTitle(QString const& title)
{
    if(title.isNull() || title.isEmpty())
    {
        kdError(23100) << "HTML doc title is null or empty!" << endl
        << toString() << endl;
    }
    Q_ASSERT(!title.isNull() && !title.isEmpty());

    has_html_doc_title_ = true;
    html_doc_title_ = title;
}

inline void LinkStatus::setIgnored(bool flag)
{
    ignored_ = flag;
}

inline void LinkStatus::setMimeType(QString const& mimetype)
{
    Q_ASSERT(!mimetype.isNull() && !mimetype.isEmpty());
    mimetype_ = mimetype;
}

inline void LinkStatus::setIsErrorPage(bool flag)
{
    is_error_page_ = flag;
}

inline void LinkStatus::setIsLocalRestrict(bool flag)
{
    is_local_restrict_ = flag;
}

inline void LinkStatus::setTreeViewItem(TreeViewItem* tree_view_item)
{
    Q_ASSERT(tree_view_item);
    tree_view_item_ = tree_view_item;
}

inline void LinkStatus::addReferrer(KURL const& url)
{
    Q_ASSERT(url.isValid());

    referrers_.push_back(url);
}




inline KURL const& LinkStatus::rootUrl() const
{
    return root_url_;
}

inline LinkStatus::Status const& LinkStatus::status() const
{
    return status_;
}

inline uint LinkStatus::depth() const
{
    return depth_;
}

inline bool LinkStatus::local() const
{
    return external_domain_depth_ == -1;
}

inline bool LinkStatus::isLocalRestrict() const
{
    return is_local_restrict_;
}

inline LinkStatus const* LinkStatus::parent() const
{
    return parent_;
}

inline QString const& LinkStatus::originalUrl() const
{
    return original_url_;
}

inline QString const& LinkStatus::label() const
{
    return label_;
}

inline KURL const& LinkStatus::absoluteUrl() const
{
    return absolute_url_;
}

inline QString const& LinkStatus::docHtml() const
{
    return doc_html_;
}

inline HttpResponseHeader const& LinkStatus::httpHeader() const
{
    return http_header_;
}

inline HttpResponseHeader& LinkStatus::httpHeader()
{
    return http_header_;
}

inline QString LinkStatus::statusText() const
{
    if(errorOccurred())
        return error();
    else if(!absoluteUrl().protocol().startsWith("http"))
        return status_text_;
    else
    {
        QString string_code = QString::number(httpHeader().statusCode());
        if(absoluteUrl().hasRef()) // ref URL
            return status_text_;
        else if(string_code == "200"/* or string_code == "304"*/)
            return "OK";
        else
            return string_code;
    }
}

inline QString const& LinkStatus::error() const
{
    return error_;
}

inline bool LinkStatus::isRoot() const
{
    return is_root_;
}

inline bool LinkStatus::errorOccurred() const
{
    return error_occurred_;
}

inline bool LinkStatus::isRedirection() const
{
    return is_redirection_;
}

inline LinkStatus* LinkStatus::redirection() const
{
    Q_ASSERT(isRedirection());
    
    return redirection_;
}

inline Node* LinkStatus::node() const
{
    //Q_ASSERT(node_);
    return node_;
}

inline vector<Node*> const& LinkStatus::childrenNodes() const
{
    return children_nodes_;
}

inline bool LinkStatus::checked() const
{
    return checked_;
}

inline int LinkStatus::externalDomainDepth() const
{
    return external_domain_depth_;
}

inline bool LinkStatus::onlyCheckHeader() const
{
    return only_check_header_;
}

inline bool LinkStatus::hasBaseURI() const
{
    return has_base_URI_;
}

inline bool LinkStatus::hasHtmlDocTitle() const
{
    return has_html_doc_title_;
}

inline KURL const& LinkStatus::baseURI() const
{
    Q_ASSERT(hasBaseURI());
    return base_URI_;
}

inline QString const& LinkStatus::htmlDocTitle() const
{
    Q_ASSERT(has_html_doc_title_);
    return html_doc_title_;
}

inline bool LinkStatus::ignored() const
{
    return ignored_;
}

inline QString LinkStatus::mimeType() const
{
    Q_ASSERT(!mimetype_.isNull());
    return mimetype_;
}

inline bool LinkStatus::isErrorPage() const
{
    return is_error_page_;
}

inline TreeViewItem* LinkStatus::treeViewItem() const
{
    return tree_view_item_;
}

inline QValueVector<KURL> const& LinkStatus::referrers() const
{
    return referrers_;
}

