/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include "klinkstatus.h"

#include <kshortcutsdialog.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kedittoolbar.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <ktoolbar.h>


KLinkStatus::KLinkStatus()
  : KParts::MainWindow(0, Qt::Window)
{
    // set the shell's ui resource file
    setXMLFile("klinkstatus_shell.rc");

    setupActions();

    // and a status bar
    statusBar()->show();

    // this routine will find and load our Part.  it finds the Part by
    // name which is a bad idea usually.. but it's alright in this
    // case since our Part is made for this Shell
    KLibFactory *factory = KLibLoader::self()->factory("libklinkstatuspart");
    if (factory)
    {
        // now that the Part is loaded, we cast it to a Part to get
        // our hands on it
        m_part = static_cast<KParts::ReadOnlyPart *>(factory->create(this,
                 "KParts::ReadOnlyPart" ));

        if (m_part)
        {
            m_part->widget()->setFocusPolicy(Qt::ClickFocus);
            // tell the KParts::MainWindow that this is indeed the main widget
            setCentralWidget(m_part->widget());
            setStandardToolBarMenuEnabled(true);

            // and integrate the part's GUI with the shell's
            createGUI(m_part);
            removeDuplicatedActions();
        }
    }
    else
    {
        // if we couldn't find our Part, we exit since the Shell by
        // itself can't do anything useful
        KMessageBox::error(this, i18n("Could not find the KLinkStatus part; did you configure with '--prefix=/$KDEDIR' and perform 'make install'?"));
        kapp->quit();
        // we return here, cause kapp->quit() only means "exit the
        // next time we enter the event loop...
        return;
    }

    // apply the saved mainwindow settings, if any, and ask the mainwindow
    // to automatically save settings if changed: window size, toolbar
    // position, icon size, etc.
    setAutoSaveSettings();
}

KLinkStatus::~KLinkStatus()
{}

void KLinkStatus::load(const KUrl& url)
{
    m_part->openUrl(url);
}

void KLinkStatus::setupActions()
{
    //     KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
    // The above causes a segfault when using File->Quit.
    // Here's Waldo's explanation:
/*    I had a look. The problem is due to the SessionWidget destructor calling
    KLSConfig. If you use the window button, the window and the SessionWidget is
    destructed first and then later the application is destructed.
    If you use File->Quit it calls kapp->quit which destructs the application
    without destructing the window first. The application first destructs all
    static deleters and its administration, and then goes on to kill the
    remaining windows that it owns. Therein lies the problem because by then the
    static deleters aren't usable any longer, and calling KLSConfig from the
    SessionWidget destructor crashes when it tries to recreate KLSConfig and
    register it with staticKLSConfigDeleter due to the lack of static deleter
    administration.
    The easiest solution is to call close() on the mainwindow instead of
    KApplication::quit()*/
    KStandardAction::quit(this, SLOT(close()), actionCollection());

    //m_toolbarAction = KStandardAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
    //m_statusbarAction = KStandardAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

    KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
}

void KLinkStatus::removeDuplicatedActions()
{
    KActionCollection* part_action_collection = m_part->actionCollection();
    QAction* part_about_action = part_action_collection->action("about_klinkstatus");
    QAction* part_report_action = part_action_collection->action("report_bug");

    QWidget* container = part_about_action->associatedWidgets().value(0); // call this only once or segfault
    container->removeAction(part_about_action);
    container->removeAction(part_report_action);
    part_action_collection->removeAction(part_about_action);
    part_action_collection->removeAction(part_report_action);
}

void KLinkStatus::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        setStandardToolBarMenuEnabled(true);
    else
        setStandardToolBarMenuEnabled(false);
}

void KLinkStatus::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
/*    else
        statusBar()->hide();*/
}

void KLinkStatus::optionsConfigureKeys()
{
    //KShortcutsDialog::configure(actionCollection());

    KShortcutsDialog dlg(KShortcutsEditor::AllActions, KShortcutsEditor::LetterShortcutsDisallowed, this);
    QList<KXMLGUIClient*> clients = guiFactory()->clients();
	foreach(KXMLGUIClient *client, clients)
		dlg.addCollection(client->actionCollection()/*, client->componentData().aboutData()->programName() */);

    dlg.configure();
}

void KLinkStatus::optionsConfigureToolbars()
{
    saveMainWindowSettings(KGlobal::config()->group( autoSaveGroup()) );

    // use the standard toolbar editor
    KEditToolBar dlg(factory());
    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(applyNewToolbarConfig()));
    dlg.exec();
}

void KLinkStatus::applyNewToolbarConfig()
{
    applyMainWindowSettings(KGlobal::config()->group( autoSaveGroup()) );
}


#include "klinkstatus.moc"
