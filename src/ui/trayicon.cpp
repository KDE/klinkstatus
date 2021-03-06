/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
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

#include "trayicon.h"

#include <QWidget>

#include <kicon.h>
#include <klocale.h>
#include <kwindowsystem.h>
#include <kdebug.h>


TrayIcon* TrayIcon::m_instance = 0;

TrayIcon* TrayIcon::getInstance(QWidget* parent)
{
    if(!m_instance) {
        Q_ASSERT(parent);
        m_instance = new TrayIcon(parent);
    }
    
    return m_instance;
}

TrayIcon::TrayIcon(QWidget* parent)
    : KStatusNotifierItem(parent)
{
    setIconByName("klinkstatus");
    setToolTip("klinkstatus", i18n("KLinkStatus - Link Checker"), QString());
}

TrayIcon::~TrayIcon()
{
    kDebug(23100) << "";
}


#include "trayicon.moc"
