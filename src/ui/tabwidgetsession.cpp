/***************************************************************************
 *   Copyright (C) 2004-2007 by Paulo Moura Guedes                              *
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
    
#include "tabwidgetsession.h"
    
#include "ui/sessionwidget.h"
#include "ui/sessionstackedwidget.h"
#include "ui/treeview.h"
#include "engine/searchmanager.h"
#include "klsconfig.h"
#include "actionmanager.h"
#include "klsfactory.h"
    
#include <QToolButton>
#include <QCursor>
#include <QToolTip>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>
#include <QStringList>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kaction.h>
#include <kiconloader.h>


TabWidgetSession::TabWidgetSession(QWidget* parent, Qt::WFlags f)
        : KTabWidget(parent, f) // tabs_ is initialized with size 17
{
    setFocusPolicy(Qt::NoFocus);
    setTabReorderingEnabled(true);
    setHoverCloseButton(true);
    setHoverCloseButtonDelayed(true);
    
    tabs_.setAutoDelete(false);
    
    QToolButton* tabs_new = new QToolButton(this);
    tabs_new->setShortcut(QKeySequence("Ctrl+N"));
    connect(tabs_new, SIGNAL(clicked()), this, SLOT(slotNewSession()));
    tabs_new->setIcon(KIcon("tab-new"));
    tabs_new->adjustSize();
    tabs_new->setToolTip(i18n("Open new tab"));
    setCornerWidget(tabs_new, Qt::TopLeftCorner);

    tabs_close_ = new QToolButton(this);
    tabs_close_->setShortcut(QKeySequence("Ctrl+W"));
    connect(tabs_close_, SIGNAL(clicked()), this, SLOT(closeSession()));
    tabs_close_->setIcon(KIcon("tab-remove"));
    tabs_close_->adjustSize();
    tabs_close_->setToolTip(i18n("Close the current tab"));
    setCornerWidget(tabs_close_, Qt::TopRightCorner);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));
}

TabWidgetSession::~TabWidgetSession()
{}

SessionStackedWidget* TabWidgetSession::currentWidget() const
{
    return static_cast<SessionStackedWidget*> (QTabWidget::currentWidget());
}

SessionStackedWidget* TabWidgetSession::widget(int index) const
{
    return static_cast<SessionStackedWidget*> (QTabWidget::widget(index));
}

bool TabWidgetSession::emptySessionsExist() const
{
    if(count() == 0)
        return true;

    for(int i = 0; i != count(); ++i)
    {
        SessionWidget* sessionWidget = widget(i)->sessionWidget();
        if(sessionWidget && sessionWidget->isEmpty()
           && !sessionWidget->inProgress()) {
            return true;
        }
    }
    return false;
}

SessionWidget* TabWidgetSession::getEmptySession() const
{
    Q_ASSERT(emptySessionsExist());
    Q_ASSERT(count() != 0);

    for(int i = 0; i != count(); ++i)
    {
        SessionWidget* sessionWidget = widget(i)->sessionWidget();
        if(sessionWidget && sessionWidget->isEmpty()
          && !sessionWidget->inProgress()) {
            return sessionWidget;
        }
    }
    return 0;
}

void TabWidgetSession::addNewSession(KUrl const& url)
{
    SessionStackedWidget* page = new SessionStackedWidget(url, this);
    
    connect(page, SIGNAL(signalTitleChanged(SessionStackedWidget*)),
            this, SLOT(updateTabLabel(SessionStackedWidget*)));

    ActionManager::getInstance()->slotUpdateActions(page);

    addTab(page, i18n("Session") + QString::number(count() + 1));
    setCurrentIndex(count() - 1);
}

void TabWidgetSession::closeSession()
{
    if(count() > 1)
        removePage(currentWidget());

    tabs_close_->setEnabled(count() > 1);
    ActionManager::getInstance()->action("close_tab")->setEnabled(count() > 1);
}

void TabWidgetSession::updateTabLabel(SessionStackedWidget* page)
{
    SessionWidget* sessionWidget = page->sessionWidget();
    setTabText(indexOf(page), KCharsets::resolveEntities(sessionWidget->title()));
    setTabIcon(indexOf(page), QIconSet(KIO::pixmapForUrl(sessionWidget->urlToCheck())));
}

void TabWidgetSession::slotLoadSettings()
{
    for(int i = 0; i != count(); ++i)
    {
        SessionWidget* sessionWidget = widget(i)->sessionWidget();

        if(sessionWidget == 0)
            return;

        if(sessionWidget->isEmpty()
            && !sessionWidget->inProgress()) {
            sessionWidget->slotLoadSettings(true);
        }
        else {
            sessionWidget->slotLoadSettings(false);
        }
    }
}

void TabWidgetSession::setUrl(KUrl const& url)
{
    currentWidget()->sessionWidget()->setUrl(url);
}

void TabWidgetSession::slotCurrentChanged(int index)
{
    tabs_close_->setEnabled(count() > 1);

    ActionManager::getInstance()->slotUpdateActions(widget(index));
}

void TabWidgetSession::slotHideSearchPanel()
{
    currentWidget()->sessionWidget()->slotHideSearchPanel();
}

void TabWidgetSession::slotFollowLastLinkChecked()
{
    currentWidget()->sessionWidget()->slotFollowLastLinkChecked();
}

void TabWidgetSession::slotResetSearchOptions()
{
    currentWidget()->sessionWidget()->slotResetSearchOptions();
}

void TabWidgetSession::slotNewSession(KUrl const& url)
{
    if(count() == 0 || !emptySessionsExist())
    {
        addNewSession(url);
    }
    else
    {
        SessionWidget* sessionwidget = getEmptySession();
        setCurrentIndex(indexOf(sessionwidget));
        
        if(url.isValid())
            sessionwidget->setUrl(url);
    }

    ActionManager::getInstance()->action("close_tab")->setEnabled(count() > 1);
}

void TabWidgetSession::slotStartSearch()
{
    PlayableWidgetInterface* playable = static_cast<PlayableWidgetInterface*> (currentWidget()->currentWidget());
    playable->slotStartSearch();
}

void TabWidgetSession::slotPauseSearch()
{
    PlayableWidgetInterface* playable = static_cast<PlayableWidgetInterface*> (currentWidget()->currentWidget());
    playable->slotPauseSearch();
}

void TabWidgetSession::slotStopSearch()
{
    PlayableWidgetInterface* playable = static_cast<PlayableWidgetInterface*> (currentWidget()->currentWidget());
    playable->slotStopSearch();
}

void TabWidgetSession::slotRecheckVisibleItems()
{
    currentWidget()->sessionWidget()->slotRecheckVisibleItems();
}

void TabWidgetSession::slotRecheckBrokenItems()
{
    currentWidget()->sessionWidget()->slotRecheckBrokenItems();
}

void TabWidgetSession::slotExportAsHTML()
{
    currentWidget()->sessionWidget()->slotExportAsHTML();
}

void TabWidgetSession::slotValidateAll( )
{
    currentWidget()->sessionWidget()->slotValidateAll();
}


#include "tabwidgetsession.moc"
