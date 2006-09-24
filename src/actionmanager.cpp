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
#include <kguiitem.h>

#include <qbuttongroup.h>

#include "klinkstatus_part.h"
#include "ui/sessionwidget.h"
#include "ui/tabwidgetsession.h"
#include "cfg/klsconfig.h"


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

    new KAction(i18n("New Link Check"), "filenew",
                0,
                d->part, SLOT(slotNewLinkCheck()),
                d->actionCollection, "new_link_check");

    new KAction(i18n("Open URL..."), "fileopen",
                0,
                d->part, SLOT(slotOpenLink()),
                d->actionCollection, "open_link");

    action = new KAction(i18n("Close Tab"), "fileclose",
                         0,
                         d->part, SLOT(slotClose()),
                         d->actionCollection, "close_tab");
    action->setEnabled(false);

    // *************** Settings menu *********************

    (void) new KAction(i18n("Configure KLinkStatus..."), "configure",
                       0, d->part, SLOT(slotConfigureKLinkStatus()),
                       d->actionCollection, "configure_klinkstatus");

    // *************** Help menu *********************

    (void) new KAction(i18n("About KLinkStatus"), "klinkstatus",
                       0, d->part, SLOT(slotAbout()),
                       d->actionCollection, "about_klinkstatus");

    (void) new KAction(i18n("&Report Bug..."), 0, 0, d->part,
                       SLOT(slotReportBug()), d->actionCollection, "report_bug");

    // *************** View menu *********************
}

void ActionManager::initTabWidget(TabWidgetSession* tabWidgetSession)
{
    Q_ASSERT(tabWidgetSession);

    if (d->tabWidgetSession)
        return;

    d->tabWidgetSession = tabWidgetSession;
    
    // *************** File menu *********************

    KAction* action = new KAction(i18n("E&xport Results as HTML..."), "filesave", 0,
                                  d->tabWidgetSession, SLOT(slotExportAsHTML()),
                                  d->actionCollection, "file_export_html");
    action->setEnabled(false);

    // *************** View menu *********************

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    KToggleAction* toggle_action = new KToggleAction(i18n("&Follow last Link checked"),
                                   "make_kdevelop", "Ctrl+f",
                                   d->tabWidgetSession, SLOT(slotFollowLastLinkChecked()),
                                   d->actionCollection, "follow_last_link_checked");
    toggle_action->setChecked(KLSConfig::followLastLinkChecked());

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    toggle_action = new KToggleAction(i18n("&Hide Search Panel"), "bottom", "Ctrl+h",
                                      d->tabWidgetSession, SLOT(slotHideSearchPanel()),
                                      d->actionCollection, "hide_search_bar");
    KGuiItem item(i18n("&Show Search Panel"), "top", "Show Search Panel");
    toggle_action->setCheckedState(item);
    
    new KAction(i18n("&Reset Search Options"), "reload", "F5",
                d->tabWidgetSession, SLOT(slotResetSearchOptions()),
                d->actionCollection, "reset_search_bar");

    // *************** Search menu *********************
    
    toggle_action = new KToggleAction(i18n("&Start Search"),
                                      "player_play", "Ctrl+s",
                                      d->tabWidgetSession, SLOT(slotStartSearch()),
                                      d->actionCollection, "start_search");
    toggle_action->setEnabled(false);
    
    toggle_action = new KToggleAction(i18n("&Pause Search"),
                                      "player_pause", "Ctrl+p",
                                      d->tabWidgetSession, SLOT(slotPauseSearch()),
                                      d->actionCollection, "pause_search");    
    toggle_action->setEnabled(false);
    
    action = new KAction(i18n("St&op Search"),
                                  "player_stop", "Ctrl+c",
                                  d->tabWidgetSession, SLOT(slotStopSearch()),
                                  d->actionCollection, "stop_search");
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

KAction* ActionManager::action(const char* name, const char* classname)
{
    return d->actionCollection != 0 ? d->actionCollection->action(name, classname) : 0;
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
