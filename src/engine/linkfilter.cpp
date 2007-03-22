/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                        *
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
#include "linkfilter.h"

#include "linkstatus.h"


LinkMatcher::LinkMatcher(QString const& text, LinkStatusHelper::Status status)
    : m_text(text), m_status(status)
{
}

LinkMatcher::~LinkMatcher()
{
}

bool LinkMatcher::matches(LinkStatus const& link ) const
{
    if(!hasCriteria())
        return true;

    return
      (link.absoluteUrl().url().contains(m_text, Qt::CaseInsensitive)
        || link.label().contains(m_text, Qt::CaseInsensitive))
      && LinkStatusHelper::hasStatus(&link, m_status);
}



