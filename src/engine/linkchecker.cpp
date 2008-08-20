/***************************************************************************
 *   Copyright (C) 2004 by Puto Moura                                      *
 *   mojo@localhost.localdomain                                            *
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
#include "linkchecker.h"
#include "searchmanager.h"
#include "../utils/utils.h"
#include "../parser/htmlparser.h"

#include <qstring.h>
#include <qtimer.h>
#include <qtextcodec.h> 
#include <qcstring.h>

#include <kio/netaccess.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/scheduler.h>
#include <kio/slave.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <klocale.h>
#include <khtml_part.h>
#include <dom/html_misc.h>
#include <dom/dom_node.h>
#include <dom/dom_string.h>


int LinkChecker::count_ = 0;

LinkChecker::LinkChecker(LinkStatus* linkstatus, int time_out,
                         QObject *parent, const char *name)
        : QObject(parent, name), search_manager_(0), 
        linkstatus_(linkstatus), t_job_(0), time_out_(time_out), checker_(0), document_charset_(), 
        redirection_(false), header_checked_(false), finnished_(false), 
        parsing_(false), is_charset_checked_(false), has_defined_charset_(false)
{
    Q_ASSERT(linkstatus_);
    Q_ASSERT(!linkstatus_->checked());

    kdDebug(23100) << endl << ++count_ << ": " << "Checking " << linkstatus_->absoluteUrl().url() << endl;
}

LinkChecker::~LinkChecker()
{}

void LinkChecker::setSearchManager(SearchManager* search_manager)
{
    Q_ASSERT(search_manager);
    search_manager_ = search_manager;
}

void LinkChecker::check()
{
    Q_ASSERT(!finnished_);

    KURL url(linkStatus()->absoluteUrl());
    Q_ASSERT(url.isValid());

    if(url.hasRef())
        checkRef();
    else
    {
        t_job_ = KIO::get
                     (url, false, false);

        t_job_->addMetaData("PropagateHttpHeader", "true"); // to have the http header

        if (linkstatus_->parent()) {
            t_job_->addMetaData("referrer", linkstatus_->parent()->absoluteUrl().prettyURL());
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
        QObject::connect(t_job_, SIGNAL(result(KIO::Job *)),
                         this, SLOT(slotResult(KIO::Job *)));
        QObject::connect(t_job_, SIGNAL(redirection(KIO::Job *, const KURL &)),
                         this, SLOT(slotRedirection(KIO::Job *, const KURL &)));

        QTimer::singleShot( time_out_ * 1000, this, SLOT(slotTimeOut()) );
        
        t_job_->setInteractive(false);
    }
}

void LinkChecker::slotTimeOut()
{
    if(!finnished_ && !parsing_)
    {
        kdDebug(23100) << "timeout: " << linkstatus_->absoluteUrl().url() << endl;
        if(t_job_ && t_job_->slave())
            kdDebug(23100) << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;
        else
            kdDebug(23100) << endl;

        
//         Q_ASSERT(t_job_); // can happen: e.g. bad result signal
        if(t_job_->error() != KIO::ERR_USER_CANCELED)
        {
            linkstatus_->setErrorOccurred(true);
            linkstatus_->setChecked(true);
            linkstatus_->setError(i18n("Timeout"));
            linkstatus_->setStatus(LinkStatus::TIMEOUT);

            killJob();
            finnish();
        }
    }
}

void LinkChecker::slotMimetype (KIO::Job* /*job*/, const QString &type)
{
    if(finnished_)
        return;

//     kdDebug(23100) <<  "LinkChecker::slotMimetype:" << type << "-> " << linkstatus_->absoluteUrl().url() 
//             << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;

    Q_ASSERT(t_job_);

    LinkStatus* ls = 0;
/*    if(redirection_)
        ls = linkStatus()->redirection();
    else*/
        ls = linkstatus_;
    Q_ASSERT(ls);

    ls->setMimeType(type);
    KURL url = ls->absoluteUrl();

    // we doesn't do nothing if file is http or https because we need the header
    // which is only available in the data response
    if(!t_job_->error()) // if a error happened let result() handle that
    {
        if(ls->onlyCheckHeader())
        {
            //kdDebug(23100) <<  "only check header: " << ls->absoluteUrl().prettyURL() << endl;

            // file is OK (http can have an error page though job->error() is false)
            if(!url.protocol().startsWith("http"))
            {
                ls->setStatusText("OK");
                ls->setStatus(LinkStatus::SUCCESSFULL);
                
                killJob();                
                finnish();
            }
        }
        else // !ls->onlyCheckHeader()
        {
            //kdDebug(23100) <<  "NOT only check header: " << ls->absoluteUrl().prettyURL() << endl;

            // file is OK (http can have an error page though job->error() is false)
            if(!url.protocol().startsWith("http")) // if not, it have to go trough slotData to get the http header
            {
                // it's not an html page, so we don't want the file content
                if(type != "text/html"/* && type != "text/plain"*/)
                {
                    //kdDebug(23100) <<  "mimetype: " << type << endl;
                    ls->setStatusText("OK");
                    ls->setStatus(LinkStatus::SUCCESSFULL);
                    
                    killJob();                    
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

    kdDebug(23100) <<  "LinkChecker::slotData -> " << linkstatus_->absoluteUrl().url() 
            << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid()  << endl;
    
    Q_ASSERT(t_job_);
    
    LinkStatus* ls = 0;
/*    if(redirection_)
        ls = linkStatus()->redirection();
    else*/
        ls = linkstatus_;
    Q_ASSERT(ls);

    KURL url = ls->absoluteUrl();

    if(!t_job_->error())
    {
        if(ls->onlyCheckHeader())
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
                    killJob();
                    linkstatus_->setStatus(getHttpStatus());
                    linkstatus_->setChecked(true);
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
                if(ls->mimeType() != "text/html" && header_checked_)
                {
                    //kdDebug(23100) <<  "mimetype of " << ls->absoluteUrl().prettyURL() << ": " << ls->mimeType() << endl;
                    ls->setStatus(getHttpStatus());
                    killJob();
                    finnish(); // if finnish is called before kill what you get is a segfault, don't know why
                    return;
                }
                else if(t_job_->isErrorPage() && header_checked_)
                {
                    //kdDebug(23100) <<  "ERROR PAGE" << endl;
                    ls->setIsErrorPage(true);
                    ls->setStatus(getHttpStatus());
                    killJob();
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
                codec = QTextCodec::codecForName(document_charset_);
            if(!codec)
                codec = QTextCodec::codecForName("iso8859-1"); // default
            
            doc_html_ += codec->toUnicode(data);
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
void LinkChecker::slotResult(KIO::Job* /*job*/)
{
    if(finnished_)
        return;

    kdDebug(23100) <<  "LinkChecker::slotResult -> " << linkstatus_->absoluteUrl().url()  << endl;

    Q_ASSERT(t_job_);
    if(!t_job_)
        return;
    
    if(redirection_) {
        if(!processRedirection(redirection_url_)) {
            t_job_ = 0;
            linkstatus_->setChecked(true);
            finnish();
            return;
        }
    }
            
    KIO::TransferJob* job = t_job_;
    t_job_ = 0;

    emit jobFinnished(this);

    if(job->error() == KIO::ERR_USER_CANCELED)
    {
        // FIXME This can happen! If the job is non interactive...
        kdWarning(23100) << endl << "Job killed quietly, yet signal result was emited..." << endl;
        kdDebug(23100) << linkstatus_->toString() << endl;
        finnish();
        return;
    }

    LinkStatus* ls = 0;
    if(redirection_)
        ls = linkStatus()->redirection();
    else
        ls = linkstatus_;
    Q_ASSERT(ls);

    if(!(!ls->onlyCheckHeader() ||
          job->error() ||
          !header_checked_))
        kdWarning(23100) << ls->toString() << endl;

    Q_ASSERT(!ls->onlyCheckHeader() || job->error() || !header_checked_);

    if(ls->isErrorPage())
        kdWarning(23100) << "\n\n" << ls->toString() << endl << endl;

    Q_ASSERT(!job->isErrorPage());

    if(job->error())
    {
        kdDebug(23100) << "Job error: " <<  job->errorString() << endl;
        kdDebug(23100) << "Job error code: " <<  job->error() << endl;

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
                kdWarning(23100) << "\n\nError string is empty, error = " << job->error() << "\n\n\n";
            if(job->error() != KIO::ERR_NO_CONTENT) 
                ls->setError(job->errorString());
            else
                ls->setError(i18n("No Content"));
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
                kdDebug(23100) <<  "\n\nheader not received... checking again...\n\n\n";
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
            HtmlParser parser(doc_html_);

            if(parser.hasBaseUrl())
                ls->setBaseURI(KURL(parser.baseUrl().url()));
            if(parser.hasTitle())
                ls->setHtmlDocTitle(parser.title().attributeTITLE());
            
            ls->setChildrenNodes(parser.nodes());
            parsing_ = false;
        }
    }
    finnish();
}


void LinkChecker::slotRedirection (KIO::Job* /*job*/, const KURL &url)
{
    kdDebug(23100) <<  "LinkChecker::slotRedirection -> " << 
            linkstatus_->absoluteUrl().url()  << " -> " << url.url() << endl;
//             << " - " << t_job_->slave() << "/" <<  t_job_->slave()->slave_pid() << endl;
    
    redirection_ = true;
    redirection_url_ = url;
}

bool LinkChecker::processRedirection(KURL const& toUrl)
{
    if(finnished_)
        return true;

    kdDebug(23100) <<  "LinkChecker::processRedirection -> " << linkstatus_->absoluteUrl().url() << " -> " << toUrl.url() << endl;

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
        kdDebug(23100) <<  "LinkChecker::finnish -> " << linkstatus_->absoluteUrl().url() << endl;

        finnished_ = true;

        if(redirection_)
            Q_ASSERT(linkstatus_->checked());
        else
            linkstatus_->setChecked(true);

        emit transactionFinished(linkstatus_, this);
    }
}

HttpResponseHeader LinkChecker::getHttpHeader(KIO::Job* /*job*/, bool remember_check)
{
    //kdDebug(23100) <<  "LinkChecker::getHttpHeader -> " << linkstatus_->absoluteUrl().url() << endl;
    
    Q_ASSERT(!finnished_);
    Q_ASSERT(t_job_);

    QString header_string = t_job_->queryMetaData("HTTP-Headers");
    //    Q_ASSERT(!header_string.isNull() && !header_string.isEmpty());
//     kdDebug(23100) << "HTTP header: " << endl << header_string << endl;
//     kdDebug(23100) << "Keys: " << HttpResponseHeader(header_string).keys() << endl;
//     kdDebug(23100) << "Content-type: " << HttpResponseHeader(header_string).contentType() << endl;
//     kdDebug(23100) << "Content-type: " << HttpResponseHeader(header_string).value("content-type") << endl;

    if(header_string.isNull() || header_string.isEmpty())
    {
        header_checked_ = false;
        kdWarning(23100) <<  "header_string.isNull() || header_string.isEmpty(): "
        << linkstatus_->toString()  << endl;
    }
    else if(remember_check)
        header_checked_ = true;

    return HttpResponseHeader(header_string);
}

void LinkChecker::checkRef()
{
    KURL url(linkStatus()->absoluteUrl());
    Q_ASSERT(url.hasRef());
    
    QString ref = url.ref();
    if(ref == "" || ref == "top") {
        linkstatus_->setStatusText("OK");
        linkstatus_->setStatus(LinkStatus::SUCCESSFULL);
        finnish();
        return;
    }

    QString url_base;
    LinkStatus const* ls_parent = 0;
    int i_ref = -1;

    if(linkStatus()->originalUrl().startsWith("#"))
        ls_parent = linkStatus()->parent();

    else
    {
        i_ref = url.url().find("#");
        url_base = url.url().left(i_ref);
        //kdDebug(23100) << "url_base: " << url_base << endl;

        Q_ASSERT(search_manager_);

        ls_parent = search_manager_->linkStatus(url_base);
    }

    if(ls_parent)
        checkRef(ls_parent);
    else
    {
        url = KURL::fromPathOrURL(url.url().left(i_ref));
        checkRef(url);
    }
}

void LinkChecker::checkRef(KURL const& url)
{
    Q_ASSERT(search_manager_);
    
    QString url_string = url.url();
    KHTMLPart* html_part = search_manager_->htmlPart(url_string);
    if(!html_part)
    {
        kdDebug() << "new KHTMLPart: " +  url_string << endl;

        html_part = new KHTMLPart();
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
            kdDebug(23100) <<  KIO::NetAccess::lastErrorString() << endl;
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
        linkstatus_->setError(i18n( "Link destination not found." ));
        linkstatus_->setStatus(LinkStatus::BROKEN);
    }

    finnish();
}

void LinkChecker::checkRef(LinkStatus const* linkstatus_parent)
{
    Q_ASSERT(search_manager_);

    QString url_string = linkstatus_parent->absoluteUrl().url();
    KHTMLPart* html_part = search_manager_->htmlPart(url_string);
    if(!html_part)
    {
        kdDebug() << "new KHTMLPart: " +  url_string << endl;

        html_part = new KHTMLPart();
        html_part->setOnlyLocalReferences(true);

        html_part->begin();
        html_part->write(linkstatus_parent->docHtml());
        html_part->end();

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
        linkstatus_->setError(i18n( "Link destination not found." ));
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
    if(!t_job_)
        return;

    KIO::TransferJob* aux = t_job_;
    t_job_ = 0;
    aux->disconnect(this);
    aux->kill(true); // quietly   
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
