//
// C++ Interface: global
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GLOBAL_H
#define GLOBAL_H

#include <qobject.h>
class QCString;

class DCOPClient;

/**
@author Paulo Moura Guedes
*/
class Global : public QObject
{
    Q_OBJECT
public:
    static Global* self();
    ~Global();

    static bool isKLinkStatusEmbeddedinQuanta();
    static bool isQuantaRunningAsUnique();
    static bool isQuantaAvailableViaDCOP();
    static QCString quantaDCOPAppId();

private:
    Global(QObject *parent = 0, const char *name = 0);
    static Global* m_self_;
    
    DCOPClient* dcop_client_;
};

#endif
