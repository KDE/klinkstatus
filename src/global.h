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

/**
@author Paulo Moura Guedes
*/
class Global : public QObject
{
    Q_OBJECT
public:
    Global(QObject *parent = 0, const char *name = 0);
    ~Global();

    static bool isQuantaAvailableViaDCOP();
    static QCString quantaDCOPAppId();
};

#endif
