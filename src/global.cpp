//
// C++ Implementation: global
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "global.h"

#include <qstring.h>

#include <dcopclient.h>
#include <dcopref.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstaticdeleter.h>
#include <kurl.h>

#include <sys/types.h>
#include <unistd.h>


Global* Global::m_self_ = 0;
static KStaticDeleter<Global> staticDeleter;


Global* Global::self()
{
    if (not m_self_)
    {
        staticDeleter.setObject(m_self_, new Global());
    }

    return m_self_;
}

Global::Global(QObject *parent, const char *name)
        : QObject(parent, name)
{
    m_self_ = this;
    dcop_client_ = kapp->dcopClient();
}

Global::~Global()
{
    if(m_self_ == this)
        staticDeleter.setObject(m_self_, 0, false);
}

bool Global::isKLinkStatusEmbeddedInQuanta()
{
    QCString app_id = "quanta-" + QCString().setNum(getpid());
    return self()->dcop_client_->isApplicationRegistered(app_id);
}

bool Global::isQuantaRunningAsUnique()
{
    return self()->dcop_client_->isApplicationRegistered("quanta");
}

bool Global::isQuantaAvailableViaDCOP()
{
    if(isQuantaRunningAsUnique() or isKLinkStatusEmbeddedInQuanta())
        return true;

    else
    {
        return false;
        //QStringList quanta_pid = quanta;
    }
}

QCString Global::quantaDCOPAppId()
{
    DCOPClient* client = kapp->dcopClient();

    if(client->isApplicationRegistered("quanta")) // quanta is unnique application
        return "quanta";

    QCString app = "quanta-";
    QCString pid = QCString().setNum(getpid());
    QCString app_id = app + pid;

    if(client->isApplicationRegistered(app_id)) // klinkstatus is running as a part inside quanta
        return app_id;
    else
    {
        kdError(23100) << "You didn't check if Global::isQuantaAvailableViaDCOP!" << endl;
        return "";
    }

    /*
        else
        {
            QCStringList apps = client->registeredApplications();
     
            for(QCStringList::ConstIterator it = apps.begin(); it != apps.end(); ++it)
            {
                if( (*it).contains("quanta-") )
                    return (*it);
            }
            kdError(23100) << "You didn't check if Global::isQuantaAvailableViaDCOP!" << endl;
            return "";
        }
    */
}

KURL Global::urlWithQuantaPreviewPrefix(KURL const& url)
{
    Q_ASSERT(isKLinkStatusEmbeddedInQuanta());
    
    DCOPRef quanta(Global::quantaDCOPAppId(),"WindowManagerIf");
    QString string_url_with_prefix = quanta.call("urlWithPreviewPrefix", url.url());
    //kdDebug(23100) << "string_url_with_prefix: " << string_url_with_prefix << endl;

    return KURL(string_url_with_prefix);
}


#include "global.moc"
