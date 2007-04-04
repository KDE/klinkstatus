/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                        *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/
#include "tabwidgetsession.h"
#include "sessionwidget.h"
#include "klsconfig.h"
#include "treeview.h"
#include "engine/searchmanager.h"
#include "actionmanager.h"
    
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
#include <kstringhandler.h> 
#include <kcharsets.h>
#include <kmimetype.h>
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

SessionWidget* TabWidgetSession::currentSession() const
{
    return tabs_[currentIndex()];
}

bool TabWidgetSession::emptySessionsExist() const
{
    if(count() == 0)
        return true;

    for(int i = 0; i != count(); ++i)
    {
        Q_ASSERT(tabs_[i]);
        if(tabs_[i]->isEmpty() && !tabs_[i]->getSearchManager()->searching())
            return true;
    }
    return false;
}

SessionWidget* TabWidgetSession::getEmptySession() const
{
    Q_ASSERT(emptySessionsExist());
    Q_ASSERT(count() != 0);

    for(uint i = 0; i != tabs_.count(); ++i)
    {
        if(tabs_[i]->isEmpty())
            return tabs_[i];
    }
    return 0;
}

// Remember to use count() and not size()
Q3IntDict<SessionWidget> const& TabWidgetSession::sessions() const
{
    return tabs_;
}

SessionWidget* TabWidgetSession::newSession()
{
    // TODO: settings: number of connections, timeout
    SessionWidget* session_widget = newSessionWidget();
    connect(session_widget, SIGNAL(signalUpdateTabLabel(const LinkStatus *, SessionWidget*)),
            this, SLOT(updateTabLabel(const LinkStatus *, SessionWidget*)));

    addTab(session_widget, i18n("Session") + QString::number(count() + 1));
#ifdef _GNUC
    #warning The line above was originally like below, perhaps something should be changed in message extraction too?
#endif
	//insertTab(session_widget, i18n("Session") + i18n(QString::number(count() + 1).ascii()));
    
    tabs_.insert(count() - 1, session_widget);
    Q_ASSERT(tabs_[count() - 1]);
    setCurrentIndex(count() - 1);

    return session_widget;
}

SessionWidget* TabWidgetSession::newSession(KUrl const& url)
{
    SessionWidget* sessionwidget = newSession();
    currentSession()->setUrl(url);

    return sessionwidget;
}

void TabWidgetSession::closeSession()
{
    if(count() > 1)
        removePage(currentWidget());

    tabs_close_->setEnabled(count() > 1);
    ActionManager::getInstance()->action("close_tab")->setEnabled(count() > 1);
}

SessionWidget* TabWidgetSession::newSessionWidget()
{
    SessionWidget* session_widget = new SessionWidget(KLSConfig::maxConnectionsNumber(), 
            KLSConfig::timeOut(), this);

    QStringList columns;
    
    columns.push_back(TreeView::URL_LABEL);
    columns.push_back(TreeView::STATUS_LABEL);
    if(KLSConfig::showMarkupStatus())
        columns.push_back(TreeView::MARKUP_LABEL);
    columns.push_back(TreeView::LINK_LABEL_LABEL);
    
    session_widget->setColumns(columns);

    // FIXME
//     session_widget->tree_view->restoreLayout(KLSConfig::self()->config(), "klinkstatus");

    return session_widget;
}

void TabWidgetSession::updateTabLabel(LinkStatus const* linkstatus, SessionWidget* page)
{
    QString label;
    KUrl url = linkstatus->absoluteUrl();
    
    if(linkstatus->hasHtmlDocTitle())
    {
        label = linkstatus->htmlDocTitle();
        label = KStringHandler::csqueeze(label, 30);
    }
    else
    {
        if(url.fileName(KUrl::ObeyTrailingSlash).isEmpty())
            label = url.prettyUrl();
        else
            label = url.fileName(KUrl::ObeyTrailingSlash);
        
        label = KStringHandler::lsqueeze(label, 30);        
    }
    
    setTabText(indexOf(page), KCharsets::resolveEntities(label));
    setTabIcon(indexOf(page), QIconSet(KIO::pixmapForUrl(url)));
}

void TabWidgetSession::slotLoadSettings()
{
    for(uint i = 0; i != tabs_.count(); ++i)
    {
        if(tabs_[i]->isEmpty())
        {
            SessionWidget* session_widget = tabs_[i];
            if(session_widget->isEmpty())
                session_widget->slotLoadSettings(true);
            else
                session_widget->slotLoadSettings(false);
        }
    }
}

void TabWidgetSession::setUrl(KUrl const& url)
{
    currentSession()->setUrl(url);
}

void TabWidgetSession::slotCurrentChanged(int)
{
    tabs_close_->setEnabled(count() > 1);

    SessionWidget* session_widget = currentSession();
    ActionManager::getInstance()->slotUpdateSessionWidgetActions(session_widget);
}

void TabWidgetSession::slotHideSearchPanel()
{
    currentSession()->slotHideSearchPanel();
}

void TabWidgetSession::slotFollowLastLinkChecked()
{
    currentSession()->slotFollowLastLinkChecked();
}

void TabWidgetSession::slotResetSearchOptions()
{
    currentSession()->slotResetSearchOptions();
}

void TabWidgetSession::slotNewSession(KUrl const& url)
{
    if(count() == 0 || !emptySessionsExist())
    {
        SessionWidget* sessionwidget = newSession(url);
        ActionManager::getInstance()->initSessionWidget(sessionwidget);        
    }
    else
    {
        SessionWidget* sessionwidget = getEmptySession();
        if(url.isValid())
            sessionwidget->setUrl(url);
        setCurrentIndex(indexOf(sessionwidget));
    }

    ActionManager::getInstance()->action("close_tab")->setEnabled(count() > 1);
}

void TabWidgetSession::slotStartSearch()
{
    currentSession()->slotStartSearch();
}

void TabWidgetSession::slotPauseSearch()
{
    currentSession()->slotPauseSearch();
}

void TabWidgetSession::slotStopSearch()
{
    currentSession()->slotStopSearch();
}

void TabWidgetSession::slotRecheckVisibleItems()
{
    currentSession()->slotRecheckVisibleItems();
}

void TabWidgetSession::slotExportAsHTML()
{
    currentSession()->slotExportAsHTML();
}

void TabWidgetSession::slotValidateAll( )
{
    currentSession()->slotValidateAll();
}

#include "tabwidgetsession.moc"
