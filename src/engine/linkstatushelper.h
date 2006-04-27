/***************************************************************************
 *   Copyright (C) 2006 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef LINKSTATUSHELPER_H
#define LINKSTATUSHELPER_H

#include <qstring.h>
class QDomElement;

class LinkStatus;

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
    
    All the code here could go to LinkStatus class.
    It's here to save some memory, as there are a lot of LinkStatus objects out there.
*/
class LinkStatusHelper
{
public:
    
    enum Status {
        none = 0,
        good,
        bad,
        malformed,
        undetermined // timeouts and refs
    };

    LinkStatusHelper(LinkStatus const* linkstatus);
    ~LinkStatusHelper();

    QString const toString() const;

    void save(QDomElement& element) const;
    
    static LinkStatus* lastRedirection(LinkStatus* ls);

    bool hasStatus(Status state) const;
    bool isGood() const;
    bool isBroken() const;
    bool isMalformed() const;
    bool isUndetermined() const;
    
private:
    LinkStatus const* linkstatus_;
};

#endif
