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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "linkchecker.h"
#include "linkstatus.h"
#include "searchmanager.h"
#include "../parser/htmlparser.h"

#include <qstring.h>
#include <qtimer.h>

#include <kio/netaccess.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/scheduler.h>
#include <kmimetype.h>


LinkChecker::LinkChecker(LinkStatus* linkstatus, int time_out,
                         QObject *parent, const char *name)
        : QObject(parent, name), //QThread(),
        linkstatus_(linkstatus), t_job_(0), time_out_(time_out), checker_(0),
        redirection_(false), header_checked_(false), finnished_(false), parsing_(false)
{
    assert(linkstatus_);
    assert(!linkstatus_->checked());
    assert(QString(parent->className()) == QString("SearchManager"));

    kdDebug(23100) <<  "Checking " << linkstatus_->absoluteUrl().url() << endl;
}

LinkChecker::~LinkChecker()
{}

void LinkChecker::check()
{
    assert(!finnished_);

    KURL url = linkStatus()->absoluteUrl();
    assert(url.isValid());

    t_job_ = KIO::get
                 (url, false, false);

    t_job_->addMetaData("PropagateHttpHeader", "true"); // to see the http header

    QObject::connect(t_job_, SIGNAL(data(KIO::Job *, const QByteArray &)),
                     this, SLOT(slotData(KIO::Job *, const QByteArray &)));
    QObject::connect(t_job_, SIGNAL(mimetype(KIO::Job *, const QString &)),
                     this, SLOT(slotMimetype(KIO::Job *, const QString &)));
    QObject::connect(t_job_, SIGNAL(result(KIO::Job *)),
                     this, SLOT(slotResult(KIO::Job *)));
    QObject::connect(t_job_, SIGNAL(permanentRedirection(KIO::Job *, const KURL &, const KURL &)),
                     this, SLOT(slotPermanentRedirection(KIO::Job *, const KURL &, const KURL &)));

    QTimer::singleShot( time_out_ * 1000, this, SLOT(slotTimeOut()) );
}

void LinkChecker::slotTimeOut()
{
    if(not finnished_ and not parsing_)
    {
        assert(t_job_);

        linkstatus_->setErrorOccurred(true);
        linkstatus_->setError("Timeout");

        t_job_->kill(true); // quietly
        t_job_ = 0;

        finnish();
    }
}
void LinkChecker::slotMimetype (KIO::Job* /*job*/, const QString &type)
{
    if(finnished_)
        return;

    //kdDebug(23100) <<  "LinkChecker::slotMimetype:" << type << "-> " << linkstatus_->absoluteUrl().url() << endl;

    assert(t_job_);

    LinkStatus* ls = 0;
    if(redirection_)
        ls = linkStatus()->redirection();
    else
        ls = linkstatus_;
    assert(ls);

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
            if(url.protocol() != "http" && url.protocol() != "https")
            {
                ls->setStatus("OK");
                t_job_->kill(true); // quietly
                t_job_ = 0;
                finnish();
            }
        }
        else // !ls->onlyCheckHeader()
        {
            //kdDebug(23100) <<  "NOT only check header: " << ls->absoluteUrl().prettyURL() << endl;

            // file is OK (http can have an error page though job->error() is false)
            if(url.protocol() != "http" && url.protocol() != "https")
            {
                // it's not an html page, so we don't want the file content
                if(type != "text/html"/* && type != "text/plain"*/)
                {
                    //kdDebug(23100) <<  "mimetype: " << type << endl;
                    ls->setStatus("OK");
                    t_job_->kill(true); // quietly
                    t_job_ = 0;
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

    //kdDebug(23100) <<  "LinkChecker::slotData -> " << linkstatus_->absoluteUrl().url()  << endl;

    assert(t_job_);

    LinkStatus* ls = 0;
    if(redirection_)
        ls = linkStatus()->redirection();
    else
        ls = linkstatus_;
    assert(ls);

    KURL url = ls->absoluteUrl();

    if(!t_job_->error())
    {
        if(ls->onlyCheckHeader())
        {
            assert(header_checked_ == false);
            // the job should had been killed in slotMimetype
            assert(url.protocol() == "http" || url.protocol() == "https");

            // get the header and quit
            if(url.protocol() == "http" || url.protocol() == "https")
            {
                // get the header
                ls->setHttpHeader(getHttpHeader(t_job_));

                if(t_job_->isErrorPage())
                    ls->setIsErrorPage(true);

                if(header_checked_)
                {
                    t_job_->kill(true); // quietly
                    t_job_ = 0;
                    finnish();
                }
            }
        }
        else
        {
            if(url.protocol() == "http" || url.protocol() == "https")
            {
                if(!header_checked_)
                {
                    ls->setHttpHeader(getHttpHeader(t_job_));
                }
                if(ls->mimeType() != "text/html" and header_checked_)
                {
                    //kdDebug(23100) <<  "mimetype of " << ls->absoluteUrl().prettyURL() << ": " << ls->mimeType() << endl;
                    t_job_->kill(true); // quietly
                    t_job_ = 0;
                    finnish(); // if finnish is called before kill what you get is a segfault, don't know why
                }
                else if(t_job_->isErrorPage() and header_checked_)
                {
                    //kdDebug(23100) <<  "ERROR PAGE" << endl;
                    ls->setIsErrorPage(true);
                    t_job_->kill(true); // quietly
                    t_job_ = 0;
                    finnish();
                }
                else
                    doc_html_ += QString(data);
            }

            else
            {
                assert(ls->mimeType() == "text/html");
                doc_html_ += QString(data);
            }
        }
    }
}

// only comes here if an error happened or in case of a clean html page
// if onlyCheckHeader is false
void LinkChecker::slotResult(KIO::Job* /*job*/)
{
    emit jobFinnished(this);

    if(finnished_)
        return;

    //kdDebug(23100) <<  "LinkChecker::slotResult -> " << linkstatus_->absoluteUrl().url()  << endl;

    assert(t_job_);

    if(t_job_->error() and t_job_->error() == KIO::ERR_USER_CANCELED)
    {
        kdWarning(23100) << "\n\nJob killed quietly, yet signal result was emited...\n\n\n";
        return;
    }

    LinkStatus* ls = 0;
    if(redirection_)
        ls = linkStatus()->redirection();
    else
        ls = linkstatus_;
    assert(ls);

    if(!(!ls->onlyCheckHeader() or
            t_job_->error() or
            not header_checked_))
        kdWarning(23100) << ls->toString() << endl;

    assert(not ls->onlyCheckHeader() or t_job_->error() or !header_checked_);

    if(ls->isErrorPage())
        kdWarning(23100) << "\n\n" << ls->toString() << endl << endl;

    assert(not t_job_->isErrorPage());

    if(t_job_->error())
    {
        kdDebug(23100) <<  t_job_->errorString() << endl;

        ls->setErrorOccurred(true);
        
		if(t_job_->errorString().isEmpty())
            kdWarning(23100) << "\n\nError string is empty, error = " << t_job_->error() << "\n\n\n";
		if(t_job_->error() != KIO::ERR_NO_CONTENT)
            ls->setError(t_job_->errorString());
        else
            ls->setError("No Content");
    }

    else
    {
        if(ls->absoluteUrl().protocol() != "http" &&
                ls->absoluteUrl().protocol() != "https")
            ls->setStatus("OK");
        else
        {
            if(not header_checked_)
            {
                kdDebug(23100) <<  "\n\nheader not received... checking again...\n\n\n";
                //check again
                check();
                return;
            }
            assert(header_checked_);
        }
        
		if(!doc_html_.isNull() && !doc_html_.isEmpty())
        {
            ls->setDocHtml(doc_html_);

            parsing_ = true;
            HtmlParser parser(doc_html_);

            if(parser.hasBaseUrl())
                ls->setBaseURI(KURL(parser.baseUrl().url()));
            ls->setChildrenNodes(parser.nodes());
            parsing_ = false;
        }
    }
    t_job_ = 0;
    finnish();
}

/*
void LinkChecker::slotRedirection (KIO::Job* job, const KURL &url)
{}
*/

void LinkChecker::slotPermanentRedirection (KIO::Job* /*job*/, const KURL &/*fromUrl*/, const KURL &toUrl)
{
    if(finnished_)
        return;

    //kdDebug(23100) <<  "LinkChecker::slotPermanentRedirection -> " << linkstatus_->absoluteUrl().url() << endl;

    assert(t_job_);
    assert(linkstatus_->absoluteUrl().protocol() == "http" ||
           linkstatus_->absoluteUrl().protocol() == "https");

    redirection_ = true;

    linkstatus_->setHttpHeader(getHttpHeader(t_job_, false));
    linkstatus_->setIsRedirection(true);
    linkstatus_->setStatus("redirection");
    linkstatus_->setChecked(true);

    LinkStatus* ls_red = new LinkStatus(toUrl);
    ls_red->setRootUrl(linkstatus_->rootUrl());

    if(!linkstatus_->onlyCheckHeader())
        ls_red->setOnlyCheckHeader(false);

    linkstatus_->setRedirection(ls_red);
    linkstatus_->redirection()->setParent(linkstatus_);
    linkstatus_->redirection()->setOriginalUrl(toUrl.url());

    SearchManager* gp = dynamic_cast<SearchManager*>(parent());
    assert(gp);

    if(gp->localDomain(ls_red->absoluteUrl()))
        ls_red->setExternalDomainDepth(-1);
    else
    {
        if(gp->localDomain(linkstatus_->absoluteUrl()))
            ls_red->setExternalDomainDepth(linkstatus_->externalDomainDepth() + 1);
        else
            ls_red->setExternalDomainDepth(linkstatus_->externalDomainDepth());
    }

    if(!toUrl.isValid() || gp->existUrl(toUrl))
    {
        linkStatus()->redirection()->setChecked(false);
        //t_job_->kill(true); // causes the terrible segfault bug
        t_job_ = 0;
        finnish();
    }
    else
        linkStatus()->redirection()->setChecked(true);
}

void LinkChecker::finnish()
{
    assert(!t_job_);

    if(!finnished_)
    {
        //kdDebug(23100) <<  "LinkChecker::finnish -> " << linkstatus_->absoluteUrl().url() << endl;

        finnished_ = true;

        if(redirection_)
            assert(linkstatus_->checked());
        else
            linkstatus_->setChecked(true);

        emit transactionFinished(linkstatus_, this);
    }
}

HttpResponseHeader LinkChecker::getHttpHeader(KIO::Job* /*job*/, bool remember_check)
{
    //kdDebug(23100) <<  "LinkChecker::getHttpHeader -> " << linkstatus_->absoluteUrl().url() << endl;

    assert(!finnished_);
    assert(t_job_);

    QString header_string = t_job_->queryMetaData("HTTP-Headers");
    //    assert(!header_string.isNull() && !header_string.isEmpty());
    //kdDebug(23100) <<  header_string << endl;
    if(header_string.isNull() || header_string.isEmpty())
    {
        header_checked_ = false;
        kdDebug(23100) <<  "header_string.isNull() || header_string.isEmpty(): "
        << linkstatus_->toString()  << endl;
    }
    else if(remember_check)
        header_checked_ = true;

    return HttpResponseHeader(header_string);
}

#include "linkchecker.moc"
