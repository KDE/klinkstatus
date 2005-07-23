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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <kaboutdata.h>
#include <klocale.h>
#include <kinstance.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <kparts/genericfactory.h>
#include <kparts/factory.h>
#include <kstandarddirs.h>
#include <kaboutapplication.h>
#include <kbugreport.h>
#include <kconfigdialog.h>
#include <kglobalsettings.h>
#include <kshortcut.h>
#include <kaccel.h>
#include <kkeydialog.h>

#include "global.h"
#include "cfg/klsconfig.h"
#include "klinkstatus_part.h"
#include "ui/tabwidgetsession.h"
#include "ui/sessionwidget.h"
#include "ui/settings/configsearchdialog.h"
#include "ui/settings/configresultsdialog.h"


const char KLinkStatusPart::description_[] = I18N_NOOP( "A Link Checker" );
const char KLinkStatusPart::version_[] = "0.2.2";

// Factory code for KDE 3
typedef KParts::GenericFactory<KLinkStatusPart> KLinkStatusFactory;
K_EXPORT_COMPONENT_FACTORY( libklinkstatuspart, KLinkStatusFactory )

KLinkStatusPart::KLinkStatusPart(QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name,
                                 const QStringList & /*string_list*/)
        : KParts::ReadOnlyPart(parent, name)
{
    setInstance( KLinkStatusFactory::instance() );

    m_dlgAbout = 0;

    tabwidget_ = new TabWidgetSession( parentWidget, widgetName );
    setWidget(tabwidget_);

    initGUI();

    // we are not modified since we haven't done anything yet
    setModified(false);

    openURL("");
}

KLinkStatusPart::~KLinkStatusPart()
{}

void KLinkStatusPart::initGUI()
{
    setXMLFile("klinkstatus_part.rc", true);

    // *************** File menu *********************

    action_new_link_check_ = new KAction(i18n("New Link Check"), "filenew",
                                         0,
                                         this, SLOT(slotNewLinkCheck()),
                                         actionCollection(), "new_link_check");

    action_open_link_ = new KAction(i18n("Open URL..."), "fileopen",
                                    0,
                                    this, SLOT(slotOpenLink()),
                                    actionCollection(), "open_link");

    action_close_tab_ = new KAction(i18n("Close Tab"), "fileclose",
                                    0,
                                    this, SLOT(slotClose()),
                                    actionCollection(), "close_tab");
    action_close_tab_->setEnabled(false);

    // *************** Settings menu *********************
    /*
    (void) new KToggleAction(i18n("Show &Toolbar"),
                       0, this, SLOT(slotShowToolbar()),
                       actionCollection(), "options_show_toolbar");
    */
    /*
    (void) new KAction(i18n("Configure S&hortcuts..."),
                       0, this, SLOT(slotConfigureShortcuts()),
                       actionCollection(), "options_configure_keybinding");
    */
    /*
    (void) new KAction(i18n("Configure Tool&bars..."),
                       0, this, SLOT(slotConfigureToolbars()),
                       actionCollection(), "options_configure_toolbars");
    */
    (void) new KAction(i18n("Configure KLinkStatus..."), "configure",
                       0, this, SLOT(slotConfigureKLinkStatus()),
                       actionCollection(), "configure_klinkstatus");

    // *************** View menu *********************

    action_display_all_links_ = new KAction(i18n("All Links"), "",
                                            0, this, SLOT(slotDisplayAllLinks()),
                                            actionCollection(), "display_all_links");
    action_display_all_links_->setEnabled(false);

    action_display_good_links_ = new KAction(i18n("Good Links"), "ok",
                                 0, this, SLOT(slotDisplayGoodLinks()),
                                 actionCollection(), "display_good_links");
    action_display_good_links_->setEnabled(false);

    action_display_bad_links_ = new KAction(i18n("Broken Links"), "no",
                                            0, this, SLOT(slotDisplayBadLinks()),
                                            actionCollection(), "display_bad_links");
    action_display_bad_links_->setEnabled(false);

    action_display_malformed_links_ = new KAction(i18n("Malformed Links"),
                                      "bug",
                                      0, this, SLOT(slotDisplayMalformedLinks()),
                                      actionCollection(), "display_malformed_links");
    action_display_malformed_links_->setEnabled(false);

    action_display_undetermined_links_ = new KAction(i18n("Undetermined Links"), "help",
                                         0, this, SLOT(slotDisplayUndeterminedLinks()),
                                         actionCollection(), "display_undetermined_links");
    action_display_undetermined_links_->setEnabled(false);

    // *************** Help menu *********************

    (void) new KAction(i18n("About KLinkStatus"), "klinkstatus",
                       0, this, SLOT(slotAbout()),
                       actionCollection(), "about_klinkstatus");

    (void) new KAction(i18n("&Report Bug..."), 0, 0, this,
                       SLOT(slotReportBug()), actionCollection(), "report_bug");
}

void KLinkStatusPart::setModified(bool modified)
{
    // get a handle on our Save action and make sure it is valid
    KAction *save = actionCollection()->action(KStdAction::stdName(KStdAction::Save));
    if (!save)
        return;

    // if so, we either enable or disable it based on the current
    // state
    if (modified)
        save->setEnabled(true);
    else
        save->setEnabled(false);
}

bool KLinkStatusPart::openURL(KURL const& url)
{
    KURL url_aux = url;

    if(KLSConfig::useQuantaUrlPreviewPrefix() && Global::isKLinkStatusEmbeddedInQuanta())
    {
        url_aux = Global::urlWithQuantaPreviewPrefix(url);
        if(!url_aux.isValid() || url_aux.isEmpty())
            url_aux = url;
    }
    else
        url_aux = url;

    if(tabwidget_->count() == 0 || !tabwidget_->emptySessionsExist() )
    {
        SessionWidget* sessionwidget = tabwidget_->newSession(url_aux);
        connect(sessionwidget, SIGNAL(signalSearchFinnished()),
                this, SLOT(slotEnableDisplayLinksActions()));
        connect(sessionwidget, SIGNAL(signalSearchStarted()),
                this, SLOT(slotDisableDisplayLinksActions()));
    }
    else
    {
        tabwidget_->getEmptySession()->setUrl(url_aux);
    }

    action_close_tab_->setEnabled(tabwidget_->count() > 1);

    return true;
}

bool KLinkStatusPart::openFile()
{
    return false;
}

void KLinkStatusPart::slotNewLinkCheck()
{
    openURL("");
}

void KLinkStatusPart::slotOpenLink()
{
    QString file_name = KFileDialog::getOpenURL().url();

    if (file_name.isEmpty() == false)
    {
        openURL(file_name);
    }
}

void KLinkStatusPart::slotClose()
{
    tabwidget_->closeSession();

    if(tabwidget_->count() > 1)
        Q_ASSERT(action_close_tab_->isEnabled());
    else
        action_close_tab_->setEnabled(false);
}

void KLinkStatusPart::slotConfigureKLinkStatus()
{
    KConfigDialog *dialog = new KConfigDialog(tabwidget_, "klsconfig", KLSConfig::self());
    dialog->addPage(new ConfigSearchDialog(0, "config_search_dialog"), i18n("Check"), "viewmag");
    dialog->addPage(new ConfigResultsDialog(0, "config_results_dialog"), i18n("Results"), "player_playlist");
    dialog->show();
    connect(dialog, SIGNAL(settingsChanged()), tabwidget_, SLOT(slotLoadSettings()));
}

void KLinkStatusPart::slotDisplayAllLinks()
{
    tabwidget_->currentSession()->displayAllLinks();

    action_display_all_links_->setEnabled(false);

    action_display_good_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
}

void KLinkStatusPart::slotDisplayGoodLinks()
{
    tabwidget_->currentSession()->displayGoodLinks();

    action_display_good_links_->setEnabled(false);

    action_display_all_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
}

void KLinkStatusPart::slotDisplayBadLinks()
{
    tabwidget_->currentSession()->displayBadLinks();

    action_display_bad_links_->setEnabled(false);

    action_display_all_links_->setEnabled(true);
    action_display_good_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
}

void KLinkStatusPart::slotDisplayMalformedLinks()
{
    tabwidget_->currentSession()->displayMalformedLinks();

    action_display_malformed_links_->setEnabled(false);

    action_display_all_links_->setEnabled(true);
    action_display_good_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
}

void KLinkStatusPart::slotDisplayUndeterminedLinks()
{
    tabwidget_->currentSession()->displayUndeterminedLinks();

    action_display_undetermined_links_->setEnabled(false);

    action_display_all_links_->setEnabled(true);
    action_display_good_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
}

void KLinkStatusPart::slotEnableDisplayLinksActions()
{
    if(KLSConfig::displayTreeView())
        return;
    
    action_display_good_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
}

void KLinkStatusPart::slotDisableDisplayLinksActions()
{
    action_display_good_links_->setEnabled(false);
    action_display_bad_links_->setEnabled(false);
    action_display_malformed_links_->setEnabled(false);
    action_display_undetermined_links_->setEnabled(false);
}

void KLinkStatusPart::slotAbout()
{
    if(m_dlgAbout == 0)
    {
        m_dlgAbout = new KAboutApplication(createAboutData(), tabwidget_, "about_app");
        if(m_dlgAbout == 0)
            return;
    }

    if(!m_dlgAbout->isVisible())
    {
        m_dlgAbout->show();
    }
    else
    {
        m_dlgAbout->raise();
    }
}

void KLinkStatusPart::slotReportBug()
{
    KAboutData aboutData("klinkstatus", I18N_NOOP("KLinkStatus"), version_);
    KBugReport bugReportDlg(0, true, &aboutData);
    bugReportDlg.exec();
}
/*
void KLinkStatusPart::slotShowToolbar()
{
    KToggleAction* action_show_toolbar =
            static_cast<KToggleAction*> (actionCollection()->action("options_show_toolbar"));
    if(action_show_toolbar)
    {
        if(action_show_toolbar->isChecked())
            toolBar()->show();
        else
            toolBar()->hide();
    }
}

void KLinkStatusPart::slotConfigureShortcuts()
{
    KKeyDialog::configure(actionCollection());
}
*/
/*
void KLinkStatusPart::slotConfigureToolbars()
{
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());

    // use the standard toolbar editor
    KEditToolbar dlg(factory());
    connect(&dlg, SIGNAL(newToolbarConfig()),
             this, SLOT(applyNewToolbarConfig()));
    dlg.exec();
}

void KLinkStatusPart::applyNewToolbarConfig()
{
    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}
*/
KAboutData* KLinkStatusPart::createAboutData()
{
    KAboutData * about = new KAboutData("klinkstatuspart", I18N_NOOP("KLinkStatus Part"), version_,
                                        description_, KAboutData::License_GPL_V2,
                                        "(C) 2004 Paulo Moura Guedes", 0, 0, "moura@kdewebdev.org");

    about->addAuthor("Paulo Moura Guedes", 0, "moura@kdewebdev.org");

    about->addCredit("Manuel Menezes de Sequeira", 0, 0, "http://home.iscte.pt/~mms/");
    about->addCredit("Gonçalo Silva", 0, "gngs@paradigma.co.pt");
    about->addCredit("Nuno Monteiro", 0, 0, "http://www.itsari.org");
    about->addCredit("Eric Laffoon", 0, "sequitur@kde.org");
    about->addCredit("Andras Mantia", 0, "amantia@kde.org");
    about->addCredit("Michal Rudolf", 0, "mrudolf@kdewebdev.org");
    about->addCredit("Mathieu Kooiman", 0, " quanta@map-is.nl");
    about->addCredit("Jens Herden", 0, "jens@kdewebdev.org");

    KGlobal::dirs()->addResourceType("appicon",KStandardDirs::kde_default("data") + "klinkstatuspart/pics/");

    return about;
}

#include "klinkstatus_part.moc"
