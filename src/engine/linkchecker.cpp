/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
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
 
#include "linkchecker.h"
#include "linkstatushelper.h"
#include "searchmanager.h"
#include "utils/utils.h"
#include "parser/htmlparser.h"
#include "klsconfig.h"

#include <QString>
#include <QTimer>
#include <qtextcodec.h> 

#include <kio/netaccess.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/scheduler.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <khtml_part.h>
#include <dom/html_misc.h>
#include <dom/dom_node.h>
#include <dom/dom_string.h>
#include <dom/dom_html.h>


LinkChecker::LinkChecker(LinkStatus* linkstatus, int time_out,
                         QObject* parent)
        : QObject(parent), search_manager_(0),
        linkstatus_(linkstatus), t_job_(0), time_out_(time_out), document_charset_(),
        redirection_(false), header_checked_(false), finnished_(false), 
        parsing_(false), is_charset_checked_(false), has_defined_charset_(false)
{
//     static int i = 0;
    Q_ASSERT(linkstatus_);
//     Q_ASSERT(!linkstatus_->checked()); // rechecks are now implemented

    kDebug(23100) << "Checking " << linkstatus_->absoluteUrl().url();

//     kDebug(23100) << "LinkChecker::LinkChecker " << ++i;
}

LinkChecker::~LinkChecker()
{
//     static int i = 0;
//     kDebug(23100) << "LinkChecker::~LinkChecker " << ++i;
    
    delete t_job_;
    t_job_ = 0;
}

void LinkChecker::setSearchManager(SearchManager* search_manager)
{
    Q_ASSERT(search_manager);
    search_manager_ = search_manager;
}

void LinkChecker::check()
{
    Q_ASSERT(!finnished_);

    KUrl url(linkStatus()->absoluteUrl());
    Q_ASSERT(url.isValid());

    if(url.hasRef()) {
        KMimeType::Ptr mimeType = KMimeType::findByUrl(url);
        if(mimeType->is("text/html") || mimeType->is("application/xml")) {
            QTimer::singleShot(0, this, SLOT(slotCheckRef()));
            return;
        }
    }
        
    t_job_ = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
//         t_job_->setUiDelegate(0);

    t_job_->addMetaData("PropagateHttpHeader", "true"); // to have the http header

    if(linkstatus_->parent()) {
        t_job_->addMetaData("referrer", linkstatus_->parent()->absoluteUrl().prettyUrl());
    }

    if(search_manager_->sendIdentification())
    {
        t_job_->addMetaData("SendUserAgent", "true");
        t_job_->addMetaData("UserAgent", search_manager_->userAgent());
    }
    else
        t_job_->addMetaData("SendUserAgent", "false");


    QObject::connect(t_job_, SIGNAL(data(KIO::Job *, const QByteArray &)),
                      this, SLOT(slotData(KIO::Job *, const QByteArray &)));
    QObject::connect(t_job_, SIGNAL(mimetype(KIO::Job *, const QString &)),
                      this, SLOT(slotMimetype(KIO::Job *, const QString &)));
    QObject::connect(t_job_, SIGNAL(result(KJob *)),
                      this, SLOT(slotResult(KJob *)));
    QObject::connect(t_job_, SIGNAL(redirection(KIO::Job *, const KUrl &)),
                      this, SLOT(slotRedirection(KIO::Job *, const KUrl &)));

    QTimer::singleShot(time_out_ * 1000, this, SLOT(slotTimeOut()));
}

void LinkChecker::httpPost(QString const& postUrlString, QByteArray const& postData)
{
    kDebug(23100) << "LinkChecker::httpPost";
  
    Q_ASSERT(!finnished_);

    KUrl url(linkStatus()->absoluteUrl(), postUrlString);
    Q_ASSERT(url.isValid());

    t_job_ = KIO::http_post(url, postData, KIO::HideProgressInfo);

    t_job_->addMetaData("PropagateHttpHeader", "true"); // to have the http header
    
    if(linkstatus_->parent()) {
        t_job_->addMetaData("referrer", linkstatus_->parent()->absoluteUrl().prettyUrl());
    }

    if(search_manager_->sendIdentification())
    {
        t_job_->addMetaData("SendUserAgent", "true");
        t_job_->addMetaData("UserAgent", search_manager_->userAgent());
    }
    else {
        t_job_->addMetaData("SendUserAgent", "false");
    }
        
    QObject::connect(t_job_, SIGNAL(data(KIO::Job *, const QByteArray &)),
                      this, SLOT(slotData(KIO::Job *, const QByteArray &)));
    QObject::connect(t_job_, SIGNAL(mimetype(KIO::Job *, const QString &)),
                      this, SLOT(slotMimetype(KIO::Job *, const QString &)));
    QObject::connect(t_job_, SIGNAL(result(KJob *)),
                      this, SLOT(slotResult(KJob *)));
    QObject::connect(t_job_, SIGNAL(redirection(KIO::Job *, const KUrl &)),
                      this, SLOT(slotRedirection(KIO::Job *, const KUrl &)));

    QTimer::singleShot(time_out_ * 1000, this, SLOT(slotTimeOut()));
}


void LinkChecker::slotTimeOut()
{
    if(!finnished_ && !parsing_)
    {
        //kDebug(23100) << "timeout: " << linkstatus_->absoluteUrl().url() << " - " 
        //        << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;
        
        Q_ASSERT(t_job_);
        if(t_job_->error() != KIO::ERR_USER_CANCELED)
        {
            if(redirection_) {
                processRedirection(redirection_url_);
            }
            linkstatus_->setErrorOccurred(true);
//             linkstatus_->setChecked(true);
            linkstatus_->setError("Timeout");
            linkstatus_->setStatus(LinkStatus::TIMEOUT);

            killJob();
            kDebug(23100) << "LinkChecker::slotTimeOut - " << linkstatus_->absoluteUrl().url();
            finnish();
        }
    }
}

void LinkChecker::slotMimetype (KIO::Job* /*job*/, const QString &type)
{
    if(finnished_)
        return;

//     kDebug(23100) <<  "LinkChecker::slotMimetype:" << type << "-> " << linkstatus_->absoluteUrl().url() 
//             << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;

    Q_ASSERT(t_job_);

    LinkStatus* ls = 0;
/*    if(redirection_)
        ls = linkStatus()->redirection();
    else*/
        ls = linkstatus_;
    Q_ASSERT(ls);

    ls->setMimeType(type);
    KUrl url = ls->absoluteUrl();

    // we doesn't do nothing if file is http or https because we need the header
    // which is only available in the data response
    if(!t_job_->error()) // if a error happened let result() handle that
    {
        if(ls->onlyCheckHeader() && !KLSConfig::showMarkupStatus())
        {
            //kDebug(23100) <<  "only check header: " << ls->absoluteUrl().prettyUrl();

            // file is OK (http can have an error page though job->error() is false)
            if(!url.protocol().startsWith("http"))
            {
                ls->setStatusText("OK");
                ls->setStatus(LinkStatus::SUCCESSFULL);
                
                killJob();                
                kDebug(23100) << "LinkChecker::slotMimetype - " << linkstatus_->absoluteUrl().url();
                finnish();
            }
        }
        else // !ls->onlyCheckHeader()
        {
            //kDebug(23100) <<  "NOT only check header: " << ls->absoluteUrl().prettyUrl();

            // file is OK (http can have an error page though job->error() is false)
            if(!url.protocol().startsWith("http")) // if not, it have to go trough slotData to get the http header
            {
                // it's not an html page, so we don't want the file content
                if(type != "text/html"/* && type != "text/plain"*/)
                {
                    //kDebug(23100) <<  "mimetype: " << type;
                    ls->setStatusText("OK");
                    ls->setStatus(LinkStatus::SUCCESSFULL);
                    
                    killJob();                    
                    kDebug(23100) << "LinkChecker::slotMimetype - " << linkstatus_->absoluteUrl().url();
                    finnish();
                }
            }
        }
    }
}

void LinkChecker::slotData(KIO::Job* /*job*/, const QByteArray& data)
{   
    if(finnished_)
        return;

//     kDebug(23100) <<  "LinkChecker::slotData -> " << linkstatus_->absoluteUrl().url() 
//             << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid()  << endl;
    
    Q_ASSERT(t_job_);
    
    LinkStatus* ls = 0;
/*    if(redirection_)
        ls = linkStatus()->redirection();
    else*/
        ls = linkstatus_;
    Q_ASSERT(ls);

    KUrl url = ls->absoluteUrl();

    if(!t_job_->error())
    {
        if(ls->onlyCheckHeader() && !KLSConfig::showMarkupStatus())
        {
            Q_ASSERT(header_checked_ == false);
            // the job should have been killed in slotMimetype
            Q_ASSERT(url.protocol() == "http" || url.protocol() == "https");

            // get the header and quit
            if(url.protocol().startsWith("http"))
            {
                // get the header
                ls->setHttpHeader(getHttpHeader(t_job_));

                if(t_job_->isErrorPage())
                    ls->setIsErrorPage(true);

                if(header_checked_)
                {
                    if(redirection_) {
                        processRedirection(redirection_url_);
                    }
                    killJob();
                    linkstatus_->setStatus(getHttpStatus());
//                     linkstatus_->setChecked(true);
//                     kDebug(23100) << "LinkChecker::slotData 1 - " << linkstatus_->absoluteUrl().url();
                    finnish();
                    return;
                }
            }
        }
        else
        {
            if(url.protocol().startsWith("http"))
            {
                if(!header_checked_)
                {
                    ls->setHttpHeader(getHttpHeader(t_job_));                    
                }
                if((ls->mimeType() != "text/html" && ls->mimeType() != "application/xml" )
                    && header_checked_)
                {
                    kDebug(23100) <<  "mimetype of " << ls->absoluteUrl().url() << ": "
                        << ls->mimeType();

                    ls->setStatus(getHttpStatus());
                    if(redirection_) {
                        processRedirection(redirection_url_);
                    }
                    killJob();
//                     kDebug(23100) << "LinkChecker::slotData 2 - " << linkstatus_->absoluteUrl().url();
                    finnish(); // if finnish is called before kill what you get is a segfault, don't know why
                    return;
                }
                else if(t_job_->isErrorPage() && header_checked_)
                {
                    //kDebug(23100) <<  "ERROR PAGE";
                    ls->setIsErrorPage(true);
                    ls->setStatus(getHttpStatus());
//                     ls->setChecked(true);
                    if(redirection_) {
                        processRedirection(redirection_url_);
                    }
                    killJob();
//                     kDebug(23100) << "LinkChecker::slotData 3 - " << linkstatus_->absoluteUrl().url();
                    finnish();
                    return;
                }
            }
            else
            {
                Q_ASSERT(ls->mimeType() == "text/html");
            }
            if(!is_charset_checked_) 
                findDocumentCharset(data);
            
            QTextCodec* codec = 0;
            if(has_defined_charset_) 
                codec = QTextCodec::codecForName(document_charset_.toLatin1());
            if(!codec)
                codec = QTextCodec::codecForName("iso8859-1"); // default
            
            doc_html_.append(codec->toUnicode(data));
        }
    }
}

void LinkChecker::findDocumentCharset(QString const& doc)
{
    Q_ASSERT(!is_charset_checked_);
    
    is_charset_checked_ = true; // only check the first stream of data
                    
    if(header_checked_)
        document_charset_ = linkstatus_->httpHeader().charset();

    // try to look in the meta elements                    
    if(document_charset_.isNull() || document_charset_.isEmpty()) 
        document_charset_ = HtmlParser::findCharsetInMetaElement(doc);
    
    if(!document_charset_.isNull() && !document_charset_.isEmpty())
        has_defined_charset_ = true;
}

// only comes here if an error happened or in case of a clean html page
// if onlyCheckHeader is false
void LinkChecker::slotResult(KJob* /*job*/)
{
    if(finnished_)
        return;

    kDebug(23100) <<  "LinkChecker::slotResult -> " << linkstatus_->absoluteUrl().url();

    Q_ASSERT(t_job_);
    if(!t_job_)
        return;
    
    if(redirection_) {
        if(!processRedirection(redirection_url_)) {
            t_job_ = 0;
//             linkstatus_->setChecked(true);
            finnish();
            return;
        }
    }
            
    KIO::TransferJob* job = t_job_;
    t_job_ = 0;

    emit jobFinnished(this);

    if(job->error() == KIO::ERR_USER_CANCELED)
    {
        kWarning(23100) << endl << "Job killed quietly, yet signal result was emitted...";
        kDebug(23100) << LinkStatusHelper::toString(linkstatus_);

//         kDebug(23100) << "LinkChecker::slotResult - " << linkstatus_->absoluteUrl().url();
        finnish();
        return;
    }

    LinkStatus* ls = 0;
    if(redirection_)
        ls = linkStatus()->redirection();
    else
        ls = linkstatus_;
    Q_ASSERT(ls);

//     if(!((!ls->onlyCheckHeader() || (ls->onlyCheckHeader()
//            && KLSConfig::showMarkupStatus()))
//         || job->error()
//         || !header_checked_))
//         kWarning(23100) << LinkStatusHelper::toString(ls);

    Q_ASSERT(
        (!ls->onlyCheckHeader() || (ls->onlyCheckHeader() && KLSConfig::showMarkupStatus()))
        || job->error()
        || !header_checked_);

//     if(ls->isErrorPage())
//         kWarning(23100) << "\n\n" << LinkStatusHelper::toString(ls) << endl;

    // This can happen, Job Error code:  111
    // http://www.wi-fi.org/OpenSection/index.asp
//     Q_ASSERT(!job->isErrorPage());

    if(job->error())
    {
        kDebug(23100) << "Job Error: " <<  job->errorString();
        kDebug(23100) << "Job Error code: " <<  job->error();

        if(job->error() == KIO::ERR_IS_DIRECTORY)
        {
            ls->setStatusText("OK");
            ls->setStatus(LinkStatus::SUCCESSFULL);
        }
        else
        {
            ls->setErrorOccurred(true);
            if(job->error() == KIO::ERR_SERVER_TIMEOUT)
                ls->setStatus(LinkStatus::TIMEOUT);
            else
                ls->setStatus(LinkStatus::BROKEN);

            if(job->errorString().isEmpty())
                kWarning(23100) << "\n\nError string is empty, error = " << job->error() << "\n\n\n";
            if(job->error() != KIO::ERR_NO_CONTENT) 
                ls->setError(job->errorString());
            else
                ls->setError("No Content");
        }
    }

    else
    {
        if(!ls->absoluteUrl().protocol().startsWith("http")) {
            ls->setStatusText("OK");
            ls->setStatus(LinkStatus::SUCCESSFULL);
           }
        else
        {
            if(!header_checked_)
            {
                kDebug(23100) <<  "\n\nheader not received... checking again...\n\n\n";
                //check again
                check();
                return;
            }
            Q_ASSERT(header_checked_);

            ls->setStatus(getHttpStatus());
        }

        if(!doc_html_.isNull() && !doc_html_.isEmpty())
        {
            ls->setDocHtml(doc_html_);

            parsing_ = true;
//             HtmlParser parser(doc_html_);
// 
//             if(parser.hasBaseUrl())
//                 ls->setBaseURI(KUrl(parser.baseUrl().url()));
//             if(parser.hasTitle())
//                 ls->setHtmlDocTitle(parser.title().attributeTITLE());
//             
//             ls->setChildrenNodes(parser.nodes());
            parsing_ = false;
        }
    }
//     kDebug(23100) << "LinkChecker::slotResult - " << linkstatus_->absoluteUrl().url();
    finnish();
}

void LinkChecker::slotRedirection (KIO::Job* /*job*/, const KUrl &url)
{
    kDebug(23100) <<  "LinkChecker::slotRedirection -> " <<
            linkstatus_->absoluteUrl().url()  << " -> " << url.url() << endl;
//             << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;
    
    redirection_ = true;
    redirection_url_ = url;
}

bool LinkChecker::processRedirection(KUrl const& toUrl)
{
    if(finnished_)
        return true;

    kDebug(23100) <<  "LinkChecker::processRedirection -> " << linkstatus_->absoluteUrl().url()
        << " -> " << toUrl.url() << endl;

    Q_ASSERT(t_job_);
    Q_ASSERT(linkstatus_->absoluteUrl().protocol().startsWith("http"));
    Q_ASSERT(redirection_);

    linkstatus_->setHttpHeader(getHttpHeader(t_job_, false));
    linkstatus_->setIsRedirection(true);
    linkstatus_->setStatusText("redirection");
    linkstatus_->setStatus(LinkStatus::HTTP_REDIRECTION);
    linkstatus_->setChecked(true);
    
    LinkStatus* ls_red = new LinkStatus(*linkstatus_);
    ls_red->setAbsoluteUrl(toUrl);
    ls_red->setRootUrl(linkstatus_->rootUrl());

    if(!linkstatus_->onlyCheckHeader())
        ls_red->setOnlyCheckHeader(false);

    linkstatus_->setRedirection(ls_red);
    ls_red->setParent(linkstatus_);
    ls_red->setOriginalUrl(toUrl.url());

    Q_ASSERT(search_manager_);

    if(search_manager_->localDomain(ls_red->absoluteUrl()))
        ls_red->setExternalDomainDepth(-1);
    else
    {
        if(search_manager_->localDomain(linkstatus_->absoluteUrl()))
            ls_red->setExternalDomainDepth(linkstatus_->externalDomainDepth() + 1);
        else
            ls_red->setExternalDomainDepth(linkstatus_->externalDomainDepth());
    }

    if(!toUrl.isValid() || search_manager_->existUrl(toUrl, linkstatus_->absoluteUrl()))
    {
        ls_red->setChecked(false);
        return false;
    }
    else
    {
        ls_red->setChecked(true);
        return true;
    }
}

void LinkChecker::finnish()
{
    Q_ASSERT(!t_job_);

    if(!finnished_)
    {
        kDebug(23100) <<  "LinkChecker::finnish - " << linkstatus_->absoluteUrl().url();

        finnished_ = true;

        if(redirection_) {
            // processRedirection already called
            Q_ASSERT(linkstatus_->checked());
            linkstatus_->setChecked(true);
        }
        else {
            linkstatus_->setChecked(true);
        }

          emit transactionFinished(linkstatus_, this);
    }
}

HttpResponseHeader LinkChecker::getHttpHeader(KIO::Job* /*job*/, bool remember_check)
{
//     kDebug(23100) <<  "LinkChecker::getHttpHeader -> " << linkstatus_->absoluteUrl().url() << endl;
    
    Q_ASSERT(!finnished_);
    Q_ASSERT(t_job_);

    QString header_string = t_job_->queryMetaData("HTTP-Headers");
    //    Q_ASSERT(!header_string.isNull() && !header_string.isEmpty());
//     kDebug(23100) << "HTTP header: " << endl << header_string;
//     kDebug(23100) << "Keys: " << HttpResponseHeader(header_string).keys();
//     kDebug(23100) << "Content-type: " << HttpResponseHeader(header_string).contentType();
//     kDebug(23100) << "Content-type: " << HttpResponseHeader(header_string).value("content-type");

    if(header_string.isNull() || header_string.isEmpty())
    {
        header_checked_ = false;
        kWarning(23100) <<  "header_string.isNull() || header_string.isEmpty(): "
            << LinkStatusHelper::toString(linkstatus_) << endl;
    }
    else if(remember_check)
        header_checked_ = true;

    if(header_string == "HTTP-CACHE") {
        return HttpResponseHeader(304);
    }

    return HttpResponseHeader(header_string);
}

void LinkChecker::slotCheckRef()
{
    KUrl url(linkStatus()->absoluteUrl());
    Q_ASSERT(url.hasRef());
    
    QString ref = url.ref();
    if(ref.isEmpty() || ref == "top") {
        linkstatus_->setStatusText("OK");
        linkstatus_->setStatus(LinkStatus::SUCCESSFULL);
        kDebug(23100) << "LinkChecker::slotCheckRef - " << linkstatus_->absoluteUrl().url();
        finnish();
        return;
    }

    QString url_base;
    LinkStatus const* ls_parent = 0;
    int i_ref = -1;

    if(linkStatus()->originalUrl().startsWith('#'))
        ls_parent = linkStatus()->parent();

    else
    {
        i_ref = url.url().indexOf('#');
        url_base = url.url().left(i_ref);
//         kDebug(23100) << "url_base: " << url_base;

        Q_ASSERT(search_manager_);

        ls_parent = search_manager_->linkStatus(url_base);
    }

    if(ls_parent && ls_parent->checked() && ls_parent->docHtml().isEmpty())
          checkRef(ls_parent);
    else
    {
        url = KUrl(url.url().left(i_ref));
        checkRef(url);
    }
}

void LinkChecker::checkRef(KUrl const& url)
{
//     kDebug(23100) << "LinkChecker::checkRef(KUrl const&): " << ++checkedRefHeavy;
    Q_ASSERT(search_manager_);
    
    QString url_string = url.url();
    KHTMLPart* html_part = search_manager_->htmlPart(url_string);
    if(!html_part)
    {
//         kDebug(23100) << "new KHTMLPart: " +  url_string;

        QWidget* w = 0;
        html_part = new KHTMLPart(w, this);
//         html_part = new KHTMLPart();
        html_part->setJScriptEnabled(false);
        html_part->setJavaEnabled(false);
        html_part->setMetaRefreshEnabled(false);
        html_part->setPluginsEnabled(false);
        html_part->setOnlyLocalReferences(true);

        QString tmpFile;
        if(KIO::NetAccess::download(url, tmpFile, 0))
        {
            QString doc_html = FileManager::read(tmpFile);
            html_part->begin();
            html_part->write(doc_html);
            html_part->end();

            KIO::NetAccess::removeTempFile(tmpFile);
        } 
        else 
        {
            kError(23100) <<  KIO::NetAccess::lastErrorString() << endl;
        }

        search_manager_->addHtmlPart(url_string, html_part);
    }

    if(hasAnchor(html_part, linkStatus()->absoluteUrl().ref()))
    {
        linkstatus_->setStatusText("OK");
        linkstatus_->setStatus(LinkStatus::SUCCESSFULL);
    }
    else
    {
        linkstatus_->setErrorOccurred(true);
        linkstatus_->setError("Link destination not found.");
        linkstatus_->setStatus(LinkStatus::BROKEN);
    }

//     kDebug(23100) << "LinkChecker::checkRef - " << linkstatus_->absoluteUrl().url();
    finnish();
}

void LinkChecker::checkRef(LinkStatus const* linkstatus_parent)
{
//     kDebug(23100) << "LinkChecker::checkRef(LinkStatus const*): " << ++checkedRefLight;
    
    Q_ASSERT(search_manager_);

    bool found = false;
    
    QList<Node*> const& children_nodes = linkstatus_parent->childrenNodes();
    for(int i = 0; i != children_nodes.size(); ++i) {
        if(children_nodes[i]->element() == Node::A) {
            NodeA* nodeA = static_cast<NodeA*> (children_nodes[i]);
            if(nodeA->attributeNAME() == linkStatus()->absoluteUrl().ref()) {
                found = true;
            }
        }
    }

    if(found) {
        linkstatus_->setStatusText("OK");
        linkstatus_->setStatus(LinkStatus::SUCCESSFULL);
    }
    else {
        linkstatus_->setErrorOccurred(true);
        linkstatus_->setError("Link destination not found.");
        linkstatus_->setStatus(LinkStatus::BROKEN);
    }

    finnish();
}

bool LinkChecker::hasAnchor(KHTMLPart* html_part, QString const& anchor)
{
    DOM::HTMLDocument htmlDocument = html_part->htmlDocument();
    DOM::HTMLCollection anchors = htmlDocument.anchors();

    DOM::DOMString name_ref(anchor);
    Q_ASSERT(!name_ref.isNull());

    DOM::Node node = anchors.namedItem(name_ref);
    if(node.isNull())
    {
        node = htmlDocument.getElementById(name_ref);
    }

    if(!node.isNull())
        return true;
    else
        return false;
}

void LinkChecker::killJob()
{
    KIO::TransferJob* aux = t_job_;
    t_job_ = 0;
    aux->disconnect(this);
    aux->kill(); // quietly   
}

LinkStatus::Status LinkChecker::getHttpStatus() const
{
    QString status_code = QString::number(linkstatus_->httpHeader().statusCode());
    
    if(status_code[0] == '2')
        return LinkStatus::SUCCESSFULL;
    else if(status_code[0] == '3')
        return LinkStatus::HTTP_REDIRECTION;
    else if(status_code[0] == '4')
        return LinkStatus::HTTP_CLIENT_ERROR;
    else if(status_code[0] == '5')
        return LinkStatus::HTTP_SERVER_ERROR;
    else
        return LinkStatus::UNDETERMINED;
}

#include "linkchecker.moc"
