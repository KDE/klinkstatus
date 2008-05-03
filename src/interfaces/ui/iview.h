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
 
#ifndef IVIEW_H
#define IVIEW_H

#include <QtDBus/QtDBus>

#include "klinkstatus_export.h"

class View;
class ISearchManager;

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class KLINKSTATUS_EXPORT IView : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdewebdev.klinkstatus.IView")
            
public:
    explicit IView(View* view);
    ~IView();
    
public Q_SLOTS:
    Q_SCRIPTABLE QObject* activeSearchManager();    
    
private:
    View* m_view;
};

#endif
