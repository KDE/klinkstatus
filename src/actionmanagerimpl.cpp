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
#include "actionmanagerimpl.h"

#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <klocale.h>
#include <kaction.h>
#include <kguiitem.h>

#include <qbuttongroup.h>

#include "klinkstatus_part.h"
#include "ui/sessionwidget.h"
#include "cfg/klsconfig.h"


class ActionManagerImpl::ActionManagerImplPrivate
{
public:
    ActionManagerImplPrivate()
            : part(0), sessionWidget(0)
    {}

    KActionCollection* actionCollection;

    KLinkStatusPart* part;
    SessionWidget* sessionWidget;
    //     TabWidgetSession* tabWidgetSession;
};

ActionManagerImpl::ActionManagerImpl(QObject *parent, const char *name)
        : ActionManager(parent, name), d(new ActionManagerImplPrivate)
{}

ActionManagerImpl::~ActionManagerImpl()
{
    delete d;
    d = 0;
}

void ActionManagerImpl::initPart(KLinkStatusPart* part)
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

    //     this action must be in the part because the slot can't be connected to a particular sessionWidget
    KToggleAction* toggle_action = new KToggleAction(i18n("&Follow last Link checked"),
                                   "svn_switch", "Ctrl+f",
                                   d->part, SLOT(slotFollowLastLinkChecked()),
                                   d->actionCollection, "follow_last_link_checked");
    toggle_action->setChecked(KLSConfig::followLastLinkChecked());

    //     this action must be in the part because the slot can't be connected to a particular sessionWidget
    toggle_action = new KToggleAction(i18n("&Hide Search Panel"), "bottom", "Ctrl+h",
                                      d->part, SLOT(slotHideSearchPanel()),
                                      d->actionCollection, "hide_search_bar");
    KGuiItem item(i18n("&Show Search Panel"), "top", "Show Search Panel");
    toggle_action->setCheckedState(item);

}

void ActionManagerImpl::initSessionWidget(SessionWidget* sessionWidget)
{
    Q_ASSERT(sessionWidget);

    if (d->sessionWidget)
        return;

    d->sessionWidget = sessionWidget;


    // *************** View menu *********************

    //     action_display_all_links_ = new KAction(i18n("All Links"), "",
    //                                             0, this, SLOT(slotDisplayAllLinks()),
    //                                             actionCollection(), "display_all_links");
    //     action_display_all_links_->setEnabled(false);
    //
    //     action_display_good_links_ = new KAction(i18n("Good Links"), "ok",
    //                                              0, this, SLOT(slotDisplayGoodLinks()),
    //                                              actionCollection(), "display_good_links");
    //     action_display_good_links_->setEnabled(false);
    //
    //     action_display_bad_links_ = new KAction(i18n("Broken Links"), "no",
    //                                             0, this, SLOT(slotDisplayBadLinks()),
    //                                             actionCollection(), "display_bad_links");
    //     action_display_bad_links_->setEnabled(false);
    //
    //     action_display_malformed_links_ = new KAction(i18n("Malformed Links"),
    //             "bug",
    //             0, this, SLOT(slotDisplayMalformedLinks()),
    //             actionCollection(), "display_malformed_links");
    //     action_display_malformed_links_->setEnabled(false);
    //
    //     action_display_undetermined_links_ = new KAction(i18n("Undetermined Links"), "help",
    //             0, this, SLOT(slotDisplayUndeterminedLinks()),
    //             actionCollection(), "display_undetermined_links");
    //     action_display_undetermined_links_->setEnabled(false);

    //     KToggleAction* toggle_action = new KToggleAction(i18n("&Hide Search Panel"), "bottom", "Ctrl+h",
    //                                    sessionWidget, SLOT(slotHideSearchPanel()),
    //                                    d->actionCollection, "hide_search_bar");
    //     KGuiItem item(i18n("&Show Search Panel"), "top", "Show Search Panel");
    //     //     toggle_action->setCheckedState(KGuiItem(i18n("&Show Search Panel"), "top", "Show Search Panel"));
    //     toggle_action->setCheckedState(item);

    //     toggle_action->setChecked(true);
    //     kdDebug(23100) << "toolTip: " << toggle_action->toolTip() << endl;

}

QWidget* ActionManagerImpl::container(const char* name)
{
    return d->part->factory()->container(name, d->part);
}

KActionCollection* ActionManagerImpl::actionCollection()
{
    return d->actionCollection;
}

KAction* ActionManagerImpl::action(const char* name, const char* classname)
{
    return d->actionCollection != 0 ? d->actionCollection->action(name, classname) : 0;
}

void ActionManagerImpl::slotUpdateSessionWidgetActions(QWidget* page)
{
    SessionWidget* sessionWidget = static_cast<SessionWidget*> (page);

    KToggleAction* toggleAction = static_cast<KToggleAction*> (action("follow_last_link_checked"));

    if(!toggleAction) // the first sessionWidget is created before initSessionWidget is called
    {
        initSessionWidget(sessionWidget);
        toggleAction = static_cast<KToggleAction*> (action("follow_last_link_checked"));
    }
    Q_ASSERT(toggleAction);
    toggleAction->setChecked(sessionWidget->followLastLinkChecked());

    toggleAction = static_cast<KToggleAction*> (action("hide_search_bar"));
    Q_ASSERT(toggleAction);
    toggleAction->setChecked(sessionWidget->buttongroup_search->isHidden());
}


#include "actionmanagerimpl.moc"
