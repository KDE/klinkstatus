/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
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

#include "global.h"
#include <QtDBus>
#include <QString>
#include <QTimer>
//Added by qt3to4:
#include <Q3CString>

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

Global::Global(QObject *parent)
        : QObject(parent)
{
    m_self_ = this;
}

Global::~Global()
{
    if(m_self_ == this)
        staticDeleter.setObject(m_self_, 0, false);
}

#include "global.moc"
