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

#ifndef LINKSTATUS_H
#define LINKSTATUS_H

#include "../parser/http.h"
#include "../utils/mvector.h"

#include <kurl.h>
#include <klocale.h>
#include <kdebug.h>
class TreeView;
class TreeViewItem;

#include <qstring.h>
#include <qobject.h>
#include <qvaluevector.h>
class QDomElement;

#include <vector>
#include <iostream>

using namespace std;


class Node;

class LinkStatus
{
public:

    enum Status {
        UNDETERMINED,
        SUCCESSFULL,
        BROKEN,
        HTTP_REDIRECTION,
        HTTP_CLIENT_ERROR,
        HTTP_SERVER_ERROR,
        TIMEOUT,
        NOT_SUPPORTED,
        MALFORMED
    };
    
    LinkStatus();
    LinkStatus(KURL const& absolute_url);
    LinkStatus(Node* node, LinkStatus* parent);
    ~LinkStatus();

    void save(QDomElement& element) const;
    
    void reset();
    void setRootUrl(KURL const& url);
    void setStatus(Status status);
    void setDepth(uint depth);
    void setParent(LinkStatus* parent);
    void setOriginalUrl(QString const& url_original);
    void setLabel(QString const& label);
    void setAbsoluteUrl(KURL const& url_absoluto);
    void setDocHtml(QString const& doc_html);
    void setHttpHeader(HttpResponseHeader const& cabecalho_http);
    void setStatusText(QString const& statusText); // FIXME Legacy. This should be eliminated in favor of LinkStatus::Status
    void setError(QString const& error);
    void setIsRoot(bool flag);
    void setErrorOccurred(bool houve_error);
    void setIsRedirection(bool e_redirection);
    void setRedirection(LinkStatus* redirection);
    void setNode(Node* node);
    void setChildrenNodes(vector<Node*> const& nodes);
    void addChildNode(Node* node);
    void reserveMemoryForChildrenNodes(int n);
    void setChecked(bool flag);
    void setExternalDomainDepth(int p);
    void setOnlyCheckHeader(bool flag);
    void setMalformed(bool flag = true);
    void setHasBaseURI(bool flag = true);
    void setHasHtmlDocTitle(bool flag = true);
    void setBaseURI(KURL const& base_url);
    void setHtmlDocTitle(QString const& title);
    void setIgnored(bool flag = true);
    void setMimeType(QString const& mimetype);
    void setIsErrorPage(bool flag);
    void setIsLocalRestrict(bool flag);
    void setTreeViewItem(TreeViewItem* tree_view_item);
    void addReferrer(KURL const& url);

    KURL const& rootUrl() const;
    Status const& status() const;
    uint depth() const;
    bool local() const;         // linkstatus.paradigma.co.pt == paradigma.co.pt
    bool isLocalRestrict() const; // linkstatus.paradigma.co.pt != paradigma.co.pt
    LinkStatus const* parent() const;
    QString const& originalUrl() const;
    QString const& label() const;
    KURL const& absoluteUrl() const;
    QString const& docHtml() const;
    HttpResponseHeader const& httpHeader() const;
    HttpResponseHeader& httpHeader();
    QString statusText() const; // FIXME Legacy. This should be eliminated in favor of LinkStatus::Status
    QString const& error() const;
    bool isRoot() const;
    bool errorOccurred() const;
    bool isRedirection() const;
    LinkStatus* redirection() const;
    Node* node() const;
    vector<Node*> const& childrenNodes() const;
    QString const toString() const;
    bool checked() const;
    int externalDomainDepth() const;
    bool onlyCheckHeader() const;
    bool malformed() const;
    bool hasBaseURI() const;
    bool hasHtmlDocTitle() const;
    KURL const& baseURI() const;
    QString const& htmlDocTitle() const;
    bool ignored() const;
    bool redirectionExists(KURL const& url) const; // to avoid cyclic links
    QString mimeType() const;
    bool isErrorPage() const;
    TreeViewItem* treeViewItem() const;
    QValueVector<KURL> const& referrers() const;

    static LinkStatus* lastRedirection(LinkStatus* ls);

private:

    /**
       Load some atributes in function of his parent node.
    */
    void loadNode();

private:

    KURL root_url_; // The URL which made the search start
    Status status_;
    int depth_;
    int external_domain_depth_; // Para se poder escolher explorar domains diferentes ate n depth
    QString original_url_;
    QString label_;
    KURL absolute_url_;
    QString doc_html_;
    HttpResponseHeader http_header_;
    QString status_text_; // FIXME Legacy. This should be eliminated in favor of LinkStatus::Status
    QString error_;
    bool is_root_;
    bool error_occurred_;
    bool is_redirection_;
    vector<Node*> children_nodes_;
    LinkStatus* parent_;
    LinkStatus* redirection_;
    bool checked_;
    bool only_check_header_;
    bool malformed_;
    Node* node_;
    bool has_base_URI_;
    bool has_html_doc_title_;
    KURL base_URI_;
    QString html_doc_title_;
    bool ignored_;
    QString mimetype_;
    bool is_error_page_;
    bool is_local_restrict_;
    TreeViewItem* tree_view_item_;
    QValueVector<KURL> referrers_;
};

#include "../parser/url.h"
#include "linkstatus_impl.h"

#endif
