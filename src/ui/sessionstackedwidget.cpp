/***************************************************************************
 *   Copyright (C) 2007 by Paulo Moura Guedes                              *
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

#include "sessionstackedwidget.h"

#include "klsfactory.h"
#include "actionmanager.h"
#include "ui/sessionwidget.h"
#include "ui/unreferreddocumentswidget.h"
    

SessionStackedWidget::SessionStackedWidget(KUrl const& url, QWidget* parent)
    : QStackedWidget(parent), m_sessionWidget(0), m_unreferredDocumentsWidget(0)
{
    m_sessionWidget = KLSFactory::createSessionWidget(this);

    connect(m_sessionWidget, SIGNAL(signalTitleChanged()),
            this, SLOT(slotChangeTitle()));
    connect(m_sessionWidget, SIGNAL(signalUpdateActions()),
            this, SLOT(slotUpdateActions()));

    m_sessionWidget->setUrl(url);

    addWidget(m_sessionWidget);
}

SessionStackedWidget::~SessionStackedWidget()
{
}

SessionWidget* SessionStackedWidget::sessionWidget() const
{
    return m_sessionWidget;
}

UnreferredDocumentsWidget* SessionStackedWidget::unreferredDocumentsWidget() const
{
    return m_unreferredDocumentsWidget;
}

bool SessionStackedWidget::isSessionWidgetActive() const
{
    return m_sessionWidget == currentWidget();
}

bool SessionStackedWidget::isUnreferredDocumentsWidgetActive() const
{
    return m_unreferredDocumentsWidget == currentWidget();
}

void SessionStackedWidget::slotChangeTitle()
{
    emit signalTitleChanged(this);
}

void SessionStackedWidget::slotUpdateActions()
{
    ActionManager::getInstance()->slotUpdateActions(this);
}

#include "sessionstackedwidget.moc"
