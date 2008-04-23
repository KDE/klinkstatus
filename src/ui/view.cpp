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

#include "view.h"

#include <QVBoxLayout>

#include "ui/tabwidgetsession.h"


class View::ViewPrivate
{
public:
  
    TabWidgetSession* tabWidget;
};

View::View(QWidget* parent)
    : QWidget(parent), d(new ViewPrivate())
{
    d->tabWidget = new TabWidgetSession();
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(d->tabWidget);
    
    setLayout(layout);
}

View::~View()
{
    delete d;
}

TabWidgetSession* View::sessionsTabWidget() const
{
    return d->tabWidget;
}

void View::slotNewSession(KUrl const& url)
{
    d->tabWidget->slotNewSession(url);
}

void View::closeSession()
{
    d->tabWidget->closeSession();
}

void View::slotLoadSettings()
{
    d->tabWidget->slotLoadSettings();
}



#include "view.moc"
