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

#include "iview.h"

#include "iviewadaptor.h"
#include "engine/searchmanager.h"
#include "ui/view.h"
#include "interfaces/engine/isearchmanager.h"


IView::IView(View* view)
    : QDBusAbstractAdaptor(view), m_view(view)
{
    new IViewAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
//     dbus.registerObject("/View", this);

    setAutoRelaySignals(true);
}

IView::~IView()
{
}

QObject* IView::activeSearchManager()
{
    kDebug() << "IView::activeSearchManager";
    SearchManager* m = m_view->activeSearchManager();
    kDebug() << m;
    if(!m) {
        kDebug() << "activeSearchManager is null";
        return 0;
    }
    
    return m->findChild<ISearchManager*> ();
}


#include "iview.moc"
