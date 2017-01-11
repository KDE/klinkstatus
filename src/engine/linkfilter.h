/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   kde@mouraguedes.com                                                        *
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
#ifndef LINKFILTER_H
#define LINKFILTER_H

#include <QString>

#include "linkstatushelper.h"

/**
	@author Paulo Moura Guedes <kde@mouraguedes.com>
*/
class LinkMatcher
{
public:
    LinkMatcher(QString const& text, LinkStatusHelper::Status status);
    ~LinkMatcher();

    bool matches(LinkStatus const& link) const;
    bool hasCriteria() const { return !(m_text.isEmpty() && m_status == LinkStatusHelper::none); }

    void setText(const QString& text) { m_text = text; }
    QString text() const { return m_text; }

    void setStatus(LinkStatusHelper::Status status) { m_status = status; }
    LinkStatusHelper::Status status() const { return m_status; }
        
private:
    QString m_text;
    LinkStatusHelper::Status m_status;
};

#endif
