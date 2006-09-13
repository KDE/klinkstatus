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
#include "../engine/searchmanager.h"
#include "../actionmanager.h"

#include <qtoolbutton.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qstringlist.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kstringhandler.h> 
#include <kcharsets.h>
#include <kmimetype.h>
#include <kaction.h>
#include <kiconloader.h>


TabWidgetSession::TabWidgetSession(QWidget* parent, const char* name, WFlags f)
        : KTabWidget(parent, name, f) // tabs_ is initialized with size 17
{
    setFocusPolicy(QTabWidget::NoFocus);
    setMargin(0);
    setTabReorderingEnabled(true);
    setHoverCloseButton(true);
    setHoverCloseButtonDelayed(true);
    
    tabs_.setAutoDelete(false);
    
    QToolButton* tabs_new = new QToolButton(this);
    tabs_new->setAccel(QKeySequence("Ctrl+N"));
    connect(tabs_new, SIGNAL(clicked()), this, SLOT(slotNewSession()));
    tabs_new->setIconSet(SmallIconSet("tab_new_raised"));
    tabs_new->adjustSize();
    QToolTip::add(tabs_new, i18n("Open new tab"));
    setCornerWidget(tabs_new, TopLeft);

    tabs_close_ = new QToolButton(this);
    tabs_close_->setAccel(QKeySequence("Ctrl+W"));
    connect(tabs_close_, SIGNAL(clicked()), this, SLOT(closeSession()));
    tabs_close_->setIconSet(SmallIconSet("tab_remove"));
    tabs_close_->adjustSize();
    QToolTip::add(tabs_close_, i18n("Close the current tab"));
    setCornerWidget(tabs_close_, TopRight);

    connect(this, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotCurrentChanged(QWidget*)));
}

TabWidgetSession::~TabWidgetSession()
{}

SessionWidget* TabWidgetSession::currentSession() const
{
    return tabs_[currentPageIndex()];
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
QIntDict<SessionWidget> const& TabWidgetSession::sessions() const
{
    return tabs_;
}

SessionWidget* TabWidgetSession::newSession()
{
    // TODO: settings: number of connections, timeout
    SessionWidget* session_widget = newSessionWidget();
    connect(session_widget, SIGNAL(signalUpdateTabLabel(const LinkStatus *, SessionWidget*)),
            this, SLOT(updateTabLabel(const LinkStatus *, SessionWidget*)));

    insertTab(session_widget, i18n("Session") + i18n(QString::number(count() + 1).ascii()));
    
    tabs_.insert(count() - 1, session_widget);
    Q_ASSERT(tabs_[count() - 1]);
    setCurrentPage(count() - 1);

    return session_widget;
}

SessionWidget* TabWidgetSession::newSession(KURL const& url)
{
    SessionWidget* sessionwidget = newSession();
    currentSession()->setUrl(url);

    return sessionwidget;
}

void TabWidgetSession::closeSession()
{
    if(count() > 1)
        removePage(currentPage());

    tabs_close_->setEnabled(count() > 1);
    ActionManager::getInstance()->action("close_tab")->setEnabled(count() > 1);
}

SessionWidget* TabWidgetSession::newSessionWidget()
{
    SessionWidget* session_widget = new SessionWidget(KLSConfig::maxConnectionsNumber(), 
            KLSConfig::timeOut(), this, QString("session_widget-" + count()));

    QStringList columns;
    
    columns.push_back(TreeView::URL_LABEL);
    columns.push_back(TreeView::STATUS_LABEL);
    if(KLSConfig::showMarkupStatus())
        columns.push_back(TreeView::MARKUP_LABEL);
    columns.push_back(TreeView::LINK_LABEL_LABEL);
    
    session_widget->setColumns(columns);

    session_widget->tree_view->restoreLayout(KLSConfig::self()->config(), "klinkstatus");

    return session_widget;
}

void TabWidgetSession::updateTabLabel(LinkStatus const* linkstatus, SessionWidget* page)
{
    QString label;
    KURL url = linkstatus->absoluteUrl();
    
    if(linkstatus->hasHtmlDocTitle())
    {
        label = linkstatus->htmlDocTitle();
        label = KStringHandler::csqueeze(label, 30);
    }
    else
    {
        if(url.fileName(false).isEmpty())
            label = url.prettyURL();
        else
            label = url.fileName(false);
        
        label = KStringHandler::lsqueeze(label, 30);        
    }
    
    changeTab(page, KCharsets::resolveEntities(label));
    setTabIconSet(page, QIconSet(KMimeType::pixmapForURL(url)));
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

void TabWidgetSession::setUrl(KURL const& url)
{
    currentSession()->setUrl(url);
}

void TabWidgetSession::slotCurrentChanged(QWidget* /*page*/)
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

void TabWidgetSession::slotNewSession(KURL const& url)
{
    if(count() == 0 || !emptySessionsExist())
    {
        SessionWidget* sessionwidget = newSession(url);
        ActionManager::getInstance()->initSessionWidget(sessionwidget);        
    }
    else
    {
        SessionWidget* sessionwidget = getEmptySession();
        sessionwidget->setUrl(url);
        showPage(sessionwidget);
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

void TabWidgetSession::slotExportAsHTML()
{
    currentSession()->slotExportAsHTML();
}


#include "tabwidgetsession.moc"
