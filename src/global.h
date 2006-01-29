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
class KURL;
class KProcess;

/**
@author Paulo Moura Guedes
*/
class Global : public QObject
{
    Q_OBJECT
public:
    static Global* self();
    ~Global();

    static bool isKLinkStatusEmbeddedInQuanta();
    static bool isQuantaRunningAsUnique();
    static bool isQuantaAvailableViaDCOP();
    static QCString quantaDCOPAppId();
    static KURL urlWithQuantaPreviewPrefix(KURL const& url);
    
    //static void setLoopStarted(bool flag);
    static void openQuanta(QStringList const& args);
    
private:
    Global(QObject *parent = 0, const char *name = 0);
    static void execCommand(QString const& command);

private slots:
    void slotGetScriptOutput(KProcess* process, char* buffer, int buflen);
    void slotGetScriptError(KProcess* process, char* buffer, int buflen);
    void slotProcessExited(KProcess* process);
    void slotProcessTimeout();

private:
    static Global* m_self_;

    DCOPClient* dcop_client_;
    bool loop_started_;
    QString script_output_;
    KProcess* process_PS_;
};

#endif
