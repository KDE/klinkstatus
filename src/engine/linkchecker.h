/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                                     *
 *   moura@kdewebdev.org                                            *
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
#ifndef LINKCHECKER_H
#define LINKCHECKER_H

#include <qobject.h>
#include <qthread.h>
#include <qstring.h>

#include <kio/jobclasses.h>

#include "../parser/http.h"
class LinkStatus;


#include <iostream>
using namespace std;

/**
@author Paulo Moura Guedes
*/
class LinkChecker : public QObject
{
    Q_OBJECT
public:
    LinkChecker(LinkStatus* linkstatus, int time_out = 50,
                QObject *parent = 0, const char *name = 0);
    ~LinkChecker();

    //virtual void run();
    void check();

    LinkStatus const* const linkStatus() const;

signals:

    void transactionFinished(const LinkStatus * linkstatus,
                             LinkChecker * checker);
    void jobFinnished(LinkChecker * checker);

protected slots:

    void slotData(KIO::Job *, const QByteArray &data);
    //void slotRedirection (KIO::Job *, const KURL &url);
    void slotPermanentRedirection(KIO::Job *, const KURL &fromUrl, const KURL &toUrl);
    void slotMimetype(KIO::Job *, const QString &type);
    void slotResult(KIO::Job* job);
    void slotTimeOut();

protected:

    void finnish();
    HttpResponseHeader getHttpHeader(KIO::Job* job, bool remember_check = true);
    void checkRef(); // #...

private:
    void checkRef(LinkStatus const* linkstatus_parent);

private:

    LinkStatus* const linkstatus_;
    KIO::TransferJob* t_job_;
    int time_out_;
    LinkChecker* checker_;
    bool redirection_;
    QString doc_html_;
    bool header_checked_;
    bool finnished_;
    bool parsing_;
    
    static int count_; // debug attribute that counts how many links were checked
};

inline LinkStatus const* const LinkChecker::linkStatus() const
{
    return linkstatus_;
}


#endif
