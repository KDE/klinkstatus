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

#include <qbuttongroup.h>

#include "global.h"
#include "cfg/klsconfig.h"
#include "klinkstatus_part.h"
#include "ui/tabwidgetsession.h"
#include "ui/sessionwidget.h"
#include "ui/settings/configsearchdialog.h"
#include "ui/settings/configresultsdialog.h"
#include "ui/settings/configidentificationdialog.h"
#include "actionmanager.h"


const char KLinkStatusPart::description_[] = I18N_NOOP( "A Link Checker" );
const char KLinkStatusPart::version_[] = "0.3.2";

// Factory code for KDE 3
typedef KParts::GenericFactory<KLinkStatusPart> KLinkStatusFactory;
K_EXPORT_COMPONENT_FACTORY( libklinkstatuspart, KLinkStatusFactory )

KLinkStatusPart::KLinkStatusPart(QWidget *parentWidget, const char *widgetName,
                                 QObject *parent, const char *name,
                                 const QStringList & /*string_list*/)
    : KParts::ReadOnlyPart(parent, name), m_dlgAbout(0)
{
    setInstance(KLinkStatusFactory::instance());

    action_manager_ = new ActionManager(this);
    ActionManager::setInstance(action_manager_);
    initGUI();

    tabwidget_ = new TabWidgetSession(parentWidget, widgetName);
    setWidget(tabwidget_);
    action_manager_->initTabWidget(tabwidget_);

    // we are not modified since we haven't done anything yet
    setModified(false);

    openURL("");
}

KLinkStatusPart::~KLinkStatusPart()
{}

void KLinkStatusPart::initGUI()
{
    setXMLFile("klinkstatus_part.rc", true);
    
    // initialize the part actions
    action_manager_->initPart(this);
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

    tabwidget_->slotNewSession(url_aux);

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
}

void KLinkStatusPart::slotConfigureKLinkStatus()
{
    KConfigDialog *dialog = new KConfigDialog(tabwidget_, "klsconfig", KLSConfig::self());
    dialog->addPage(new ConfigSearchDialog(0, "config_search_dialog"), i18n("Check"), "viewmag");
    dialog->addPage(new ConfigResultsDialog(0, "config_results_dialog"), i18n("Results"), "player_playlist");
    dialog->addPage(new ConfigIdentificationDialog(0), i18n("Identification"), 
                    "agent", i18n("Configure the way KLinkstatus reports itself"));
    dialog->show();
    connect(dialog, SIGNAL(settingsChanged()), tabwidget_, SLOT(slotLoadSettings()));
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

    KGlobal::dirs()->addResourceType("appicon", KStandardDirs::kde_default("data") + "klinkstatuspart/pics/");

    return about;
}

#include "klinkstatus_part.moc"
