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
#include <qtimer.h>

#include <dcopclient.h>
#include <dcopref.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kstaticdeleter.h>
#include <kurl.h>
#include <kprocess.h>

#include <sys/types.h>
#include <unistd.h>


Global* Global::m_self_ = 0;
static KStaticDeleter<Global> staticDeleter;


Global* Global::self()
{
    if (!m_self_)
    {
        staticDeleter.setObject(m_self_, new Global());
    }

    return m_self_;
}

Global::Global(QObject *parent, const char *name)
        : QObject(parent, name), loop_started_(false)
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
    if(isQuantaRunningAsUnique() || isKLinkStatusEmbeddedInQuanta())
        return true;

    else
    {
        self()->execCommand("ps h -o pid -C quanta -C quanta_be");
        QStringList ps_list = QStringList::split("\n", self()->script_output_);

        for(uint i = 0; i != ps_list.size(); ++i)
        {
            ps_list[i] = ps_list[i].stripWhiteSpace ();
            if(self()->dcop_client_->isApplicationRegistered("quanta-" + ps_list[i].local8Bit()))
            {
                //kdDebug(23100) << "Application registered!" << endl;
                return true;
            }
        }
        return false;
    }
}

QCString Global::quantaDCOPAppId()
{
    DCOPClient* client = kapp->dcopClient();
    QCString app_id;

    if(client->isApplicationRegistered("quanta")) // quanta is unnique application
        app_id = "quanta";

    else if(self()->isKLinkStatusEmbeddedInQuanta()) // klinkstatus is running as a part inside quanta
    {
        QCString app = "quanta-";
        QCString pid = QCString().setNum(getpid());
        app_id = app + pid;
    }

    else
    {
        self()->execCommand("ps h -o pid -C quanta -C quanta_be");
        QStringList ps_list = QStringList::split("\n", self()->script_output_);

        for(uint i = 0; i != ps_list.size(); ++i)
        {
            ps_list[i] = ps_list[i].stripWhiteSpace ();
            if(self()->dcop_client_->isApplicationRegistered("quanta-" + ps_list[i].local8Bit()))
                app_id = "quanta-" + ps_list[i];
        }
    }

    if(self()->dcop_client_->isApplicationRegistered(app_id))
        return app_id;
    else
    {
        kdError(23100) << "You didn't check if Global::isQuantaAvailableViaDCOP!" << endl;
        return "";
    }
}

KURL Global::urlWithQuantaPreviewPrefix(KURL const& url)
{
    Q_ASSERT(isKLinkStatusEmbeddedInQuanta());

    DCOPRef quanta(Global::quantaDCOPAppId(),"WindowManagerIf");
    QString string_url_with_prefix = quanta.call("urlWithPreviewPrefix", url.url());
    //kdDebug(23100) << "string_url_with_prefix: " << string_url_with_prefix << endl;

    return KURL(string_url_with_prefix);
}

void Global::openQuanta(QStringList const& args)
{
    QString command(args.join(" "));
    Global::execCommand("quanta " + command);    
}

void Global::execCommand(QString const& command)
{

    //We create a KProcess that executes the "ps" *nix command to get the PIDs of the
    //other instances of quanta actually running
    self()->process_PS_ = new KProcess();
    *(self()->process_PS_) << QStringList::split(" ",command);

    connect( self()->process_PS_, SIGNAL(receivedStdout(KProcess*,char*,int)),
             self(), SLOT(slotGetScriptOutput(KProcess*,char*,int)));
    connect( self()->process_PS_, SIGNAL(receivedStderr(KProcess*,char*,int)),
             self(), SLOT(slotGetScriptError(KProcess*,char*,int)));
    connect( self()->process_PS_, SIGNAL(processExited(KProcess*)),
             self(), SLOT(slotProcessExited(KProcess*)));

    //if KProcess fails I think a message box is needed... I will fix it
    if (!self()->process_PS_->start(KProcess::NotifyOnExit,KProcess::All))
        kdError() << "Failed to query for running KLinkStatus instances!" << endl;
    //TODO: Replace the above error with a real messagebox after the message freeze is over
    else
    {
        //To avoid lock-ups, start a timer.
        QTimer* timer = new QTimer(self());
        connect(timer, SIGNAL(timeout()),
                self(), SLOT(slotProcessTimeout()));
        timer->start(120*1000, true);
        self()->loop_started_ = true;
        kapp->enter_loop();
        delete timer;
    }
}

void Global::slotGetScriptOutput(KProcess* /*process*/, char* buf, int buflen)
{
    QCString tmp( buf, buflen + 1 );
    script_output_ = QString::null;
    script_output_ = QString::fromLocal8Bit(tmp).remove(" ");
}

void Global::slotGetScriptError(KProcess*, char* buf, int buflen)
{
    //TODO: Implement some error handling?
    Q_UNUSED(buf);
    Q_UNUSED(buflen);
}

void Global::slotProcessExited(KProcess*)
{
    slotProcessTimeout();
}

void Global::slotProcessTimeout()
{
    if (loop_started_)
    {
        kapp->exit_loop();
        loop_started_ = false;
    }
}


#include "global.moc"
