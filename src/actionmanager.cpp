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

ActionManager::ActionManager(QObject *parent)
        : QObject(parent), d(new ActionManagerPrivate)
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

    QAction* action = 0;

    // *************** File menu *********************

    action = d->actionCollection->addAction( "new_link_check" );
    action->setText( i18n("New Link Check") );
    action->setIcon( KIcon("filenew") );
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotNewLinkCheck()));

    action = d->actionCollection->addAction( "open_link" );
    action->setText( i18n("Open URL...") );
    action->setIcon( KIcon("fileopen") );

    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotOpenLink()));


    action = d->actionCollection->addAction( "close_tab" );
    action->setText( i18n("Close Tab") );
    action->setIcon( KIcon("fileclose") );

    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotClose()));
    action->setEnabled(false);

    // *************** Settings menu *********************

    action = d->actionCollection->addAction( "configure_klinkstatus" );
    action->setText( i18n("Configure KLinkStatus...") );
    action->setIcon( KIcon("configure") );
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotConfigureKLinkStatus()));

    // *************** Help menu *********************

    action = d->actionCollection->addAction( "about_klinkstatus" );
    action->setText( i18n("About KLinkStatus") );
    action->setIcon( KIcon("klinkstatus") );

    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotAbout()));

    action  = new KAction(i18n("&Report Bug..."), this);
    actionCollection()->addAction("report_bug", action );
    action = d->actionCollection->addAction( "report_bug" );
    action->setText( i18n("&Report Bug...") );

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

    KAction *action  = new KAction(KIcon("filesave"), i18n("E&xport Results as HTML..."), this);
    actionCollection()->addAction("file_export_html", action );
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotExportAsHTML()));
    action->setEnabled(false);

    // *************** View menu *********************

	//     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    KToggleAction *toggle_action  = new KToggleAction(KIcon("make_kdevelop"), i18n("&Follow last Link checked"), this);
    actionCollection()->addAction("follow_last_link_checked", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotFollowLastLinkChecked()));
    toggle_action->setShortcut(KShortcut("Ctrl+f"));
    toggle_action->setChecked(KLSConfig::followLastLinkChecked());

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    toggle_action  = new KToggleAction(KIcon("bottom"), i18n("&Hide Search Panel"), this);
    actionCollection()->addAction("hide_search_bar", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotHideSearchPanel()));
    toggle_action->setShortcut(KShortcut("Ctrl+h"));
    KGuiItem item(i18n("&Show Search Panel"), "top", "Show Search Panel");
    toggle_action->setCheckedState(item);

    action  = new KAction(KIcon("reload"), i18n("&Reset Search Options"), this);
    actionCollection()->addAction("reset_search_bar", action );
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotResetSearchOptions()));
    action->setShortcut(KShortcut("F5"));

    // *************** Search menu *********************

    toggle_action  = new KToggleAction(KIcon("player_play"), i18n("&Start Search"), this);
    actionCollection()->addAction("start_search", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotStartSearch()));
    toggle_action->setShortcut(KShortcut("Ctrl+s"));
    toggle_action->setEnabled(false);

    toggle_action  = new KToggleAction(KIcon("player_pause"), i18n("&Pause Search"), this);
    actionCollection()->addAction("pause_search", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotPauseSearch()));
    toggle_action->setShortcut(KShortcut("Ctrl+p"));
    toggle_action->setEnabled(false);

    action  = new KAction(KIcon("player_stop"), i18n("St&op Search"), this);
    actionCollection()->addAction("stop_search", action );
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

QAction* ActionManager::action(const QString & name)
{
    return d->actionCollection != 0 ? d->actionCollection->action(name) : 0;
}

void ActionManager::slotUpdateSessionWidgetActions(SessionWidget* page)
{
    KToggleAction* start_search_action_ = static_cast<KToggleAction*> (action("start_search"));
    KToggleAction* pause_search_action_ = static_cast<KToggleAction*> (action("pause_search"));
    QAction* stop_search_action_ = action("stop_search");

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
