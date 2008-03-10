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
 
#ifndef SEARCHMANAGERADAPTOR_H
#define SEARCHMANAGERADAPTOR_H

#include <QtDBus/QtDBus>

#include "klinkstatus_export.h"

class SearchManager;

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class KLINKSTATUS_EXPORT SearchManagerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdewebdev.klinkstatus.SearchManager")
            
public:
    explicit SearchManagerAdaptor(SearchManager* searchManager);
    ~SearchManagerAdaptor();

public Q_SLOTS:
    bool hasDocumentRoot();
    void pauseSearch();
    void resumeSearch();
    
Q_SIGNALS:
    
    
private:
    SearchManager* m_searchManager;
};

#endif
