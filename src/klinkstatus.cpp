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

#include <kkeydialog.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kedittoolbar.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kdebug.h>
#include <kaccel.h>


KLinkStatus::KLinkStatus()
        : KParts::MainWindow( 0L, "KLinkStatus" )
{
    // set the shell's ui resource file
    setXMLFile("klinkstatus_shell.rc");

    setupActions();

    // and a status bar
    //statusBar()->show();

    // this routine will find and load our Part.  it finds the Part by
    // name which is a bad idea usually.. but it's alright in this
    // case since our Part is made for this Shell
    KLibFactory *factory = KLibLoader::self()->factory("libklinkstatuspart");
    if (factory)
    {
        // now that the Part is loaded, we cast it to a Part to get
        // our hands on it
        m_part = static_cast<KParts::ReadOnlyPart *>(factory->create(this,
                 "klinkstatus_part", "KParts::ReadOnlyPart" ));

        if (m_part)
        {
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

    setupPartActions();
}

KLinkStatus::~KLinkStatus()
{}

void KLinkStatus::load(const KURL& url)
{
    m_part->openURL( url );
}

void KLinkStatus::setupActions()
{
    //     KStdAction::quit(kapp, SLOT(quit()), actionCollection()); 
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
    KStdAction::quit(this, SLOT(close()), actionCollection());

    //m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
    //m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
}

void KLinkStatus::setupPartActions()
{
    Q_ASSERT(m_part);
    KActionCollection* part_action_collection = m_part->actionCollection();
    part_action_collection->action("new_link_check")->setShortcut(KStdAccel::shortcut(KStdAccel::New));
    part_action_collection->action("open_link")->setShortcut(KStdAccel::shortcut(KStdAccel::Open));
    part_action_collection->action("close_tab")->setShortcut(KStdAccel::shortcut(KStdAccel::Close));
}

void KLinkStatus::removeDuplicatedActions()
{
    KActionCollection* part_action_collection = m_part->actionCollection();
    KAction* part_about_action = part_action_collection->action("about_klinkstatus");
    KAction* part_report_action = part_action_collection->action("report_bug");

    QWidget* container = part_about_action->container(0); // call this only once or segfault
    part_about_action->unplug(container);
    part_report_action->unplug(container);
    part_action_collection->remove(part_about_action);
    part_action_collection->remove(part_report_action);
}

void KLinkStatus::saveProperties(KConfig* /*config*/)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored
}

void KLinkStatus::readProperties(KConfig* /*config*/)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'
}

void KLinkStatus::optionsShowToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void KLinkStatus::optionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void KLinkStatus::optionsConfigureKeys()
{
    //KKeyDialog::configure(actionCollection());

    KKeyDialog dlg( false, this );
    QPtrList<KXMLGUIClient> clients = guiFactory()->clients();
    for( QPtrListIterator<KXMLGUIClient> it( clients );
            it.current(); ++it )
    {
        dlg.insert( (*it)->actionCollection() );
    }
    dlg.configure();
}

void KLinkStatus::optionsConfigureToolbars()
{
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());

    // use the standard toolbar editor
    KEditToolbar dlg(factory());
    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(applyNewToolbarConfig()));
    dlg.exec();
}

void KLinkStatus::applyNewToolbarConfig()
{
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}


#include "klinkstatus.moc"
