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

#ifndef VIEW_H
#define VIEW_H

#include <QWidget>

#include <KUrl>

#include "klinkstatus_export.h"

class TabWidgetSession;


class KLINKSTATUS_EXPORT View : public QWidget
{
Q_OBJECT
public:
    View(QWidget* parent = 0);
    ~View();
    
    TabWidgetSession* sessionsTabWidget() const;

public Q_SLOTS:
    void slotNewSession(KUrl const& url = KUrl());
    void closeSession();
    void slotLoadSettings();

private:
    class ViewPrivate;
    ViewPrivate* const d;
};

#endif
