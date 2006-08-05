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
#include "actionmanager.h"

#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <klocale.h>
#include <kaction.h>
#include <ktoggleaction.h>
#include <kguiitem.h>

#include <q3buttongroup.h>
#include <kicon.h>

#include "klinkstatus_part.h"
#include "ui/sessionwidget.h"
#include "ui/tabwidgetsession.h"
#include "klsconfig.h"


ActionManager* ActionManager::m_self = 0;

ActionManager* ActionManager::getInstance()
{
    Q_ASSERT(m_self);

    return m_self;
}

void ActionManager::setInstance(ActionManager* manager)
{
    Q_ASSERT(manager);

    m_self = manager;
}

class ActionManager::ActionManagerPrivate
{
public:
    ActionManagerPrivate()
            : part(0), tabWidgetSession(0), sessionWidget(0)
    {}

    KActionCollection* actionCollection;

    KLinkStatusPart* part;
    TabWidgetSession* tabWidgetSession;
    SessionWidget* sessionWidget;
};

ActionManager::ActionManager(QObject *parent, const char *name)
        : QObject(parent, name), d(new ActionManagerPrivate)
{}

ActionManager::~ActionManager()
{
    delete d;
    d = 0;
}

void ActionManager::initPart(KLinkStatusPart* part)
{
    Q_ASSERT(part);

    if(d->part)
        return;

    d->part = part;
    d->actionCollection = part->actionCollection();

    KAction* action = 0;

    // *************** File menu *********************

    action = new KAction(KIcon("filenew"), i18n("New Link Check"), d->actionCollection, "new_link_check");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotNewLinkCheck()));

    action = new KAction(KIcon("fileopen"), i18n("Open URL..."), d->actionCollection, "open_link");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotOpenLink()));

    action = new KAction(KIcon("fileclose"), i18n("Close Tab"), d->actionCollection, "close_tab");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotClose()));
    action->setEnabled(false);

    // *************** Settings menu *********************

    action = new KAction(KIcon("configure"), i18n("Configure KLinkStatus..."), d->actionCollection, "configure_klinkstatus");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotConfigureKLinkStatus()));

    // *************** Help menu *********************

    action = new KAction(KIcon("klinkstatus"), i18n("About KLinkStatus"), d->actionCollection, "about_klinkstatus");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotAbout()));

    action = new KAction(i18n("&Report Bug..."), d->actionCollection, "report_bug");
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotReportBug()));

    // *************** View menu *********************
}

void ActionManager::initTabWidget(TabWidgetSession* tabWidgetSession)
{
    Q_ASSERT(tabWidgetSession);

    if (d->tabWidgetSession)
        return;

    d->tabWidgetSession = tabWidgetSession;
    
    // *************** File menu *********************

    KAction *action = new KAction(KIcon("filesave"), i18n("E&xport Results as HTML..."), d->actionCollection, "file_export_html");
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotExportAsHTML()));
    action->setEnabled(false);

    // *************** View menu *********************

	//     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    KToggleAction *toggle_action = new KToggleAction(KIcon("svn_switch"), i18n("&Follow last Link checked"), d->actionCollection, "follow_last_link_checked");
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotFollowLastLinkChecked()));
    toggle_action->setShortcut(KShortcut("Ctrl+f"));
    toggle_action->setChecked(KLSConfig::followLastLinkChecked());

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    toggle_action = new KToggleAction(KIcon("bottom"), i18n("&Hide Search Panel"), d->actionCollection, "hide_search_bar");
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotHideSearchPanel()));
    toggle_action->setShortcut(KShortcut("Ctrl+h"));
    KGuiItem item(i18n("&Show Search Panel"), "top", "Show Search Panel");
    toggle_action->setCheckedState(item);
    
    action = new KAction(KIcon("reload"), i18n("&Reset Search Options"), d->actionCollection, "reset_search_bar");
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotResetSearchOptions()));
    action->setShortcut(KShortcut("F5"));

    // *************** Search menu *********************
    
    toggle_action = new KToggleAction(KIcon("player_play"), i18n("&Start Search"), d->actionCollection, "start_search");
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotStartSearch()));
    toggle_action->setShortcut(KShortcut("Ctrl+s"));
    toggle_action->setEnabled(false);
    
    toggle_action = new KToggleAction(i18n("&Pause Search"),
                                      "player_pause", KShortcut("Ctrl+p"), d->tabWidgetSession, SLOT(slotPauseSearch()), d->actionCollection, "pause_search");    
    toggle_action->setEnabled(false);
    
    action = new KAction(KIcon("player_stop"), i18n("St&op Search"), d->actionCollection, "stop_search");
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotStopSearch()));
    action->setShortcut(KShortcut("Ctrl+c"));
    action->setEnabled(false);
}

void ActionManager::initSessionWidget(SessionWidget* sessionWidget)
{
    Q_ASSERT(sessionWidget);

    if (d->sessionWidget)
        return;

    d->sessionWidget = sessionWidget;

}

QWidget* ActionManager::container(const char* name)
{
    return d->part->factory()->container(name, d->part);
}

KActionCollection* ActionManager::actionCollection()
{
    return d->actionCollection;
}

KAction* ActionManager::action(QString name)
{
    return d->actionCollection != 0 ? d->actionCollection->action(name) : 0;
}

void ActionManager::slotUpdateSessionWidgetActions(SessionWidget* page)
{
    KToggleAction* start_search_action_ = static_cast<KToggleAction*> (action("start_search"));
    KToggleAction* pause_search_action_ = static_cast<KToggleAction*> (action("pause_search"));
    KAction* stop_search_action_ = action("stop_search");
    
    if(page->inProgress())
    {
        Q_ASSERT(!page->stopped());
        
        start_search_action_->setEnabled(true);
        start_search_action_->setChecked(true);
        
        pause_search_action_->setEnabled(true);
    
        stop_search_action_->setEnabled(true);
    }
    if(page->paused())
    {
        Q_ASSERT(page->inProgress());
        Q_ASSERT(!page->stopped());
        
        start_search_action_->setEnabled(true);
        start_search_action_->setChecked(true);
        
        pause_search_action_->setEnabled(true);
        pause_search_action_->setChecked(true);

        stop_search_action_->setEnabled(true);
    }
    if(page->stopped())
    {
        Q_ASSERT(!page->inProgress());
        Q_ASSERT(!page->paused());
        
        start_search_action_->setEnabled(true);
        start_search_action_->setChecked(false);
        
        pause_search_action_->setEnabled(false);
        pause_search_action_->setChecked(false);

        stop_search_action_->setEnabled(false);
    }
    
//     ____________________________________________________________________
    
    KToggleAction* toggleAction = static_cast<KToggleAction*> (action("follow_last_link_checked"));

    if(!toggleAction) // the first sessionWidget is created before initSessionWidget is called
    {
        initSessionWidget(page);
        toggleAction = static_cast<KToggleAction*> (action("follow_last_link_checked"));
    }
    Q_ASSERT(toggleAction);
    toggleAction->setChecked(page->followLastLinkChecked());

    toggleAction = static_cast<KToggleAction*> (action("hide_search_bar"));
    Q_ASSERT(toggleAction);
    toggleAction->setChecked(page->buttongroup_search->isHidden());
    
    //     ____________________________________________________________________

    action("file_export_html")->setEnabled(!page->isEmpty());
}


#include "actionmanager.moc"
