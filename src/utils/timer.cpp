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

#include "timer.h"

#include <kdebug.h>

#include <QTimer>
#include <QTime>


Timer::Timer(QObject* delegate, QObject* parent)
    : QObject(parent), m_delegate(delegate), m_timer(0), m_interval(-1)
{
}

void Timer::start(QTime const& time, int msec)
{
    if(m_timer && m_timer->isActive()) {
        return;
    }
    
    m_interval = msec;
    
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    
    QTime currentTime = QTime::currentTime();
    int gap = -1;
    if(currentTime < time) {
        gap = currentTime.msecsTo(time);
    }
    else {
        QTime zeroTime(0, 0);
        gap = zeroTime.msecsTo(time);
        gap += time.msecsTo(currentTime);
    }
    
    kDebug(23100) << "gap seconds: " << gap / 1000;
    
    QTimer::singleShot(gap, this, SLOT(startTimer()));
}

void Timer::stop()
{
    m_timer->stop();
}

void Timer::startTimer()
{
    kDebug(23100) << "Timer::startTimer";

    kDebug(23100) << m_timer;
    Q_ASSERT(m_timer);
    
    slotTimeout();
    m_timer->start(m_interval);
}

void Timer::slotTimeout()
{
    kDebug(23100) << "Timer::slotTimeout";
    emit timeout(m_delegate);
}


#include "timer.h"
