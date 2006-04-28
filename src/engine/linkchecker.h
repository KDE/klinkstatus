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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/
#ifndef LINKCHECKER_H
#define LINKCHECKER_H

#include <qobject.h>
#include <qthread.h>
#include <qstring.h>

#include <kio/jobclasses.h>
class KHTMLPart;

#include "../parser/http.h"
class LinkStatus;
class SearchManager;

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
    void setSearchManager(SearchManager* search_manager);

    LinkStatus const* linkStatus() const;

    static bool hasAnchor(KHTMLPart* html_part, QString const& anchor);

signals:

    void transactionFinished(const LinkStatus * linkstatus,
                             LinkChecker * checker);
    void jobFinnished(LinkChecker * checker);

protected slots:

    void slotData(KIO::Job *, const QByteArray &data);
    void slotRedirection (KIO::Job *, const KUrl &url);
    void slotMimetype(KIO::Job *, const QString &type);
    void slotResult(KJob* job);
    void slotTimeOut();

protected:

    void finnish();
    HttpResponseHeader getHttpHeader(KIO::Job* job, bool remember_check = true);
    void checkRef(); // #...

private:
    void checkRef(LinkStatus const* linkstatus_parent);
    void checkRef(KUrl const& url);
    void killJob();    
    /**
     * @param url 
     * @return false if the redirection was already checked by the search manager
     */
    bool processRedirection(KUrl const& url);

private:

    SearchManager* search_manager_;
    LinkStatus* const linkstatus_;
    KIO::TransferJob* t_job_;
    int time_out_;
    LinkChecker* checker_;
/*  A redirection has appened, with the current URL. Several redirections 
    can happen until the final URL is reached.*/
    bool redirection_;
    KUrl redirection_url_;
    QString doc_html_;
    bool header_checked_;
    bool finnished_;
    bool parsing_;
    
    static int count_; // debug attribute that counts how many links were checked
};

inline LinkStatus const* LinkChecker::linkStatus() const
{
    return linkstatus_;
}


#endif
