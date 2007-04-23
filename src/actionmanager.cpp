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
#include <kstandardshortcut.h>
#include <kicon.h>

#include <q3buttongroup.h>

#include "klinkstatus_part.h"
#include "ui/sessionwidget.h"
#include "ui/sessionstackedwidget.h"    
#include "ui/tabwidgetsession.h"
#include "ui/widgetinterface.h"
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
        : part(0), tabWidgetSession(0)
    {}

    KActionCollection* actionCollection;

    KLinkStatusPart* part;
    TabWidgetSession* tabWidgetSession;
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
    action->setIcon( KIcon("document-new") );
    action->setShortcuts(KStandardShortcut::shortcut(KStandardShortcut::New));
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotNewLinkCheck()));

    action = d->actionCollection->addAction( "open_link" );
    action->setText( i18n("Open URL...") );
    action->setIcon( KIcon("document-open") );
    action->setShortcuts(KStandardShortcut::shortcut(KStandardShortcut::Open));
    connect(action, SIGNAL(triggered(bool) ), d->part, SLOT(slotOpenLink()));


    action = d->actionCollection->addAction( "close_tab" );
    action->setText(i18n("Close Tab"));
    action->setIcon(KIcon("tab-remove") );
    action->setShortcuts(KStandardShortcut::shortcut(KStandardShortcut::Close));
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

    KAction *action  = new KAction(KIcon("document-save"), i18n("E&xport Results as HTML..."), this);
    actionCollection()->addAction("file_export_html", action );
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotExportAsHTML()));
    action->setEnabled(false);

    // *************** View menu *********************

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    KToggleAction *toggle_action  = new KToggleAction(KIcon("goto-page"), i18n("&Follow last Link checked"), this);
    actionCollection()->addAction("follow_last_link_checked", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotFollowLastLinkChecked()));
    toggle_action->setShortcut(KShortcut("Ctrl+f"));
    toggle_action->setChecked(KLSConfig::followLastLinkChecked());

    //     this action must be in the tabwidget because the slot can't be connected to a particular sessionWidget
    toggle_action  = new KToggleAction(KIcon("go-bottom"), i18n("&Hide Search Panel"), this);
    actionCollection()->addAction("hide_search_bar", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotHideSearchPanel()));
    toggle_action->setShortcut(KShortcut("Ctrl+h"));
    KGuiItem item(i18n("&Show Search Panel"), "go-top", "Show Search Panel");
    toggle_action->setCheckedState(item);

    action  = new KAction(KIcon("view-refresh"), i18n("&Reset Search Options"), this);
    actionCollection()->addAction("reset_search_bar", action );
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotResetSearchOptions()));
    action->setShortcut(KShortcut("F5"));

    // *************** Search menu *********************

    toggle_action  = new KToggleAction(KIcon("media-playback-start"), i18n("&Start Search"), this);
    actionCollection()->addAction("start_search", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotStartSearch()));
    toggle_action->setShortcut(KShortcut("Ctrl+s"));
    toggle_action->setEnabled(false);

    toggle_action  = new KToggleAction(KIcon("media-playback-pause"), i18n("&Pause Search"), this);
    actionCollection()->addAction("pause_search", toggle_action );
    connect(toggle_action, SIGNAL(triggered(bool)), d->tabWidgetSession, SLOT(slotPauseSearch()));
    toggle_action->setShortcut(KShortcut("Ctrl+p"));
    toggle_action->setEnabled(false);

    action  = new KAction(KIcon("media-playback-stop"), i18n("St&op Search"), this);
    actionCollection()->addAction("stop_search", action );
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotStopSearch()));
    action->setShortcut(KShortcut("Ctrl+c"));
    action->setEnabled(false);
        
    action  = new KAction(KIcon("view-refresh"), i18n("&Broken Links"), this);
    actionCollection()->addAction("recheck_broken_items", action);
    connect(action, SIGNAL(triggered(bool) ),
            d->tabWidgetSession, SLOT(slotRecheckBrokenItems()));
    action->setEnabled(false);
        
    action  = new KAction(KIcon("view-refresh"), i18n("&Displayed Links"), this);
    actionCollection()->addAction("recheck_visible_items", action);
    connect(action, SIGNAL(triggered(bool) ),
            d->tabWidgetSession, SLOT(slotRecheckVisibleItems()));
    action->setEnabled(false);
        
    action  = new KAction(KIcon(), i18n("&Unreferred Documents..."), this);
    actionCollection()->addAction("find_unreferred_documents", action);
    connect(action, SIGNAL(triggered(bool) ),
            d->tabWidgetSession, SLOT(slotFindUnreferredDocuments()));
    action->setEnabled(false);
        
    // *************** Validate menu *********************
        
    action = new KAction(KIcon("ok"), i18n("&Fix All..."), this);
    actionCollection()->addAction("html_fix_all", action);
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotValidateAll()));
    action->setShortcut(KShortcut());
    action->setEnabled(true);

    // *************** Windows menu *********************
        
    action = new KAction(KIcon(), i18n("Previous View"), this);
    actionCollection()->addAction("goto_previous_view", action);
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotPreviousView()));
    action->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F8)));
    action->setEnabled(true);
        
    action = new KAction(KIcon(), i18n("Next View"), this);
    actionCollection()->addAction("goto_next_view", action);
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotNextView()));
    action->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::Key_F8)));
    action->setEnabled(true);

    action = new KAction(KIcon(), i18n("Previous Session"), this);
    actionCollection()->addAction("goto_previous_session", action);
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotPreviousSession()));
    action->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F6)));
    action->setEnabled(true);
        
    action = new KAction(KIcon(), i18n("Next Session"), this);
    actionCollection()->addAction("goto_next_session", action);
    connect(action, SIGNAL(triggered(bool) ), d->tabWidgetSession, SLOT(slotNextSession()));
    action->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::Key_F6)));
    action->setEnabled(true);
}

void ActionManager::initSessionWidget(SessionWidget* sessionWidget)
{
    Q_ASSERT(sessionWidget);
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

void ActionManager::slotUpdateActions(SessionStackedWidget* page)
{
    updatePlayActions(page);
    updateGeneralActions(page);
}

void ActionManager::updatePlayActions(SessionStackedWidget* page)
{
    KToggleAction* start_search_action_ = static_cast<KToggleAction*> (action("start_search"));
    KToggleAction* pause_search_action_ = static_cast<KToggleAction*> (action("pause_search"));
    QAction* stop_search_action_ = action("stop_search");
    QAction* recheck_visible_items = action("recheck_visible_items");
    QAction* recheck_broken_items = action("recheck_broken_items");

    PlayableWidgetInterface* playable = dynamic_cast<PlayableWidgetInterface*> (page->currentWidget());

    if(playable == 0)
    {
        start_search_action_->setEnabled(false);
        pause_search_action_->setEnabled(false);
        stop_search_action_->setEnabled(false);
        
        recheck_visible_items->setEnabled(false);
        recheck_broken_items->setEnabled(false);
    }
    else
    {
        if(!page->isSessionWidgetActive()) {
            recheck_visible_items->setEnabled(false);
            recheck_broken_items->setEnabled(false);
        }
        
        if(playable->inProgress())
        {
            Q_ASSERT(!playable->stopped());

            start_search_action_->setEnabled(true);
            start_search_action_->setChecked(true);

            pause_search_action_->setEnabled(playable->supportsResuming());
            pause_search_action_->setChecked(false);

            stop_search_action_->setEnabled(playable->supportsResuming());
            
            if(page->isSessionWidgetActive()) {
                recheck_visible_items->setEnabled(false);
                recheck_broken_items->setEnabled(false);
            }
        }
        // this never happens if it doesn't support resuming
        if(playable->paused())
        {
            Q_ASSERT(playable->inProgress());
            Q_ASSERT(!playable->stopped());

            start_search_action_->setEnabled(true);
            start_search_action_->setChecked(true);

            pause_search_action_->setEnabled(true);
            pause_search_action_->setChecked(true);

            stop_search_action_->setEnabled(true);

            if(page->isSessionWidgetActive()) {
                recheck_visible_items->setEnabled(true);
                recheck_broken_items->setEnabled(true);
            }
        }
        if(playable->stopped())
        {
            Q_ASSERT(!playable->inProgress());
            Q_ASSERT(!playable->paused());

            start_search_action_->setEnabled(true);
            start_search_action_->setChecked(false);

            pause_search_action_->setEnabled(false);
            pause_search_action_->setChecked(false);

            stop_search_action_->setEnabled(false);
        
            if(page->isSessionWidgetActive()) {
                recheck_visible_items->setEnabled(true);
                recheck_broken_items->setEnabled(true);
            }
        }
    }
}

void ActionManager::updateGeneralActions(SessionStackedWidget* page)
{
    KToggleAction* followAction = static_cast<KToggleAction*> (action("follow_last_link_checked"));    
    Q_ASSERT(followAction);

    KToggleAction* hideAction = static_cast<KToggleAction*> (action("hide_search_bar"));
    Q_ASSERT(hideAction);

    QAction* resetSearchOptionsAction = action("reset_search_bar");
    Q_ASSERT(resetSearchOptionsAction);

    QAction* findUnreferredDocumentsAction = action("find_unreferred_documents");
    Q_ASSERT(findUnreferredDocumentsAction);

    SessionWidget* sw = page->sessionWidget();

    followAction->setEnabled(page->isSessionWidgetActive());
    followAction->setChecked(sw->followLastLinkChecked());

    hideAction->setEnabled(page->isSessionWidgetActive());
    hideAction->setChecked(sw->searchGroupBox->isHidden());

    resetSearchOptionsAction->setEnabled(page->isSessionWidgetActive());

    findUnreferredDocumentsAction->setEnabled(!page->isUnreferredDocumentsWidgetActive());

    //     ____________________________________________________________________

    action("file_export_html")->setEnabled(!sw->isEmpty());
    action("html_fix_all")->setEnabled(!sw->isEmpty() && sw->stopped());
    action("find_unreferred_documents")->setEnabled(!sw->isEmpty() && sw->stopped());
}


#include "actionmanager.moc"
