//
// C++ Implementation: global
//
// Description:
//
//
// Author: Paulo Moura Guedes <pmg@netcabo.pt>, (C) 2004
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

#include <sys/types.h>
#include <unistd.h>


Global::Global(QObject *parent, const char *name)
        : QObject(parent, name)
{}

Global::~Global()
{}

bool Global::isQuantaAvailableViaDCOP()
{
    DCOPClient* client = kapp->dcopClient();

    if(client->isApplicationRegistered("quanta")) // quanta is unnique application
        return true;

    QCString app = "quanta-";
    QCString pid = QCString().setNum(getpid());
    QCString app_id = app + pid;
    //kdDebug(23100) << "app_id: " << app_id << endl;

    return client->isApplicationRegistered(app_id); // if true, klinkstatus is running as a part inside quanta
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
}

#include "global.moc"
