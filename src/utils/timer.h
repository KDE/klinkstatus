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

#ifndef KLSTIMER_H
#define KLSTIMER_H

#include <QObject>
class QTimer;
class QTime;

#include "klinkstatus_export.h"
             

class KLINKSTATUS_EXPORT Timer : public QObject
{
    Q_OBJECT
public:
    Timer(QObject* delegate, QObject* parent = 0);

    void start(QTime const& time, int msec);
    
Q_SIGNALS:
    void timeout(QObject*);
    
private Q_SLOTS:
    void startTimer();
    void slotTimeout();
    
private:
    QObject* m_delegate;
    QTimer* m_timer;
    int m_interval;
};

#endif
