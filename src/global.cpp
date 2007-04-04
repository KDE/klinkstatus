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
#include <QLabel>
    
#include <kdebug.h>
#include <kapplication.h>
#include <kstaticdeleter.h>
#include <kurl.h>
#include <KStatusBar>
#include <kparts/statusbarextension.h>
#include <kparts/part.h>
    
#include <sys/types.h>
#include <unistd.h>


Global* Global::m_self_ = 0;
ReadOnlyPart* Global::m_klinkStatusPart = 0;
StatusBarExtension* Global::m_statusBarExtension = 0;

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
        : QObject(parent), m_statusBarLabel(0)
{
    m_self_ = this;

    connect(&m_statusBarTimer, SIGNAL(timeout()),
            this, SLOT(slotStatusBarTimeout()));

    m_statusBarTimer.start(1000);
}

Global::~Global()
{
    if(m_self_ == this)
        staticDeleter.setObject(m_self_, 0, false);
}

void Global::setKLinkStatusPart(ReadOnlyPart* part)
{
    m_klinkStatusPart = part;

    if(part)
        m_statusBarExtension = new StatusBarExtension(part);

    m_statusBarLabel = new QLabel(statusBar());
}

KStatusBar* Global::statusBar() const
{
    if(!m_statusBarExtension)
        return 0;

    return m_statusBarExtension->statusBar();
}

void Global::setStatusBarText(QString const& text, bool permanent)
{
    if(!m_statusBarExtension)
        return;

    m_statusBarLabel->setText(text);
    m_statusBarExtension->addStatusBarItem(m_statusBarLabel, 0, permanent);

    // This is a hack for removing the added messages from the status bar.
    // Permanent seems to don't have any effect
    if(!permanent)
        QTimer::singleShot(1000 * 3, this, SLOT(slotRemoveStatusBarLabel()));
}

void Global::addStatusBarPermanentItem(QWidget* widget)
{
    if(!m_statusBarExtension)
        return;

    m_statusBarExtension->addStatusBarItem(widget, 0, true);
}

void Global::slotRemoveStatusBarLabel()
{
    m_statusBarExtension->removeStatusBarItem(m_statusBarLabel);
}

void Global::slotStatusBarTimeout()
{
    m_statusBarExtension->statusBar()->clearMessage();
}


#include "global.moc"
