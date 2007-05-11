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

#include "klinkstatus_part.h"

#include <kaboutdata.h>
#include <klocale.h>
#include <kcomponentdata.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kfiledialog.h>
#include <kparts/genericfactory.h>
#include <kparts/factory.h>
#include <kstandarddirs.h>
#include <kaboutapplicationdialog.h>
#include <kbugreport.h>
#include <kconfigdialog.h>
#include <kglobalsettings.h>
#include <kshortcut.h>

#include "ui/tabwidgetsession.h"
#include "ui/sessionwidget.h"
#include "ui_configsearchdialog.h"
#include "ui_configresultsdialog.h"
#include "ui/settings/configidentificationdialog.h"
#include "actionmanager.h"
#include "global.h"
#include "klsconfig.h"
#include "engine/interfaces/isearchmanager.h"


const char KLinkStatusPart::description_[] = I18N_NOOP( "A Link Checker" );
const char KLinkStatusPart::version_[] = "0.3.80";

// Factory code for KDE 3
typedef KParts::GenericFactory<KLinkStatusPart> KLinkStatusFactory;
K_EXPORT_COMPONENT_FACTORY( libklinkstatuspart, KLinkStatusFactory )


KLinkStatusPart::KLinkStatusPart(QWidget *parentWidget,
                                 QObject *parent,
                                 const QStringList & /*string_list*/)
    : KParts::ReadOnlyPart(parent), m_dlgAbout(0)
{
    setComponentData(KLinkStatusFactory::componentData());

    action_manager_ = new ActionManager(this);
    ActionManager::setInstance(action_manager_);
    initGUI();

    new ISearchManager(this);
    
    tabwidget_ = new TabWidgetSession(parentWidget);
    setWidget(tabwidget_);
    action_manager_->initTabWidget(tabwidget_);

    openURL(KUrl(""));
    
    Global::self()->setKLinkStatusPart(this);
}

KLinkStatusPart::~KLinkStatusPart()
{}

void KLinkStatusPart::initGUI()
{
    setXMLFile("klinkstatus_part.rc", true);

    // initialize the part actions
    action_manager_->initPart(this);
}

bool KLinkStatusPart::openURL(KUrl const& url)
{
    tabwidget_->slotNewSession(url);

    return true;
}

bool KLinkStatusPart::openFile()
{
    return false;
}

void KLinkStatusPart::slotNewLinkCheck()
{
    openURL(KUrl(""));
}

void KLinkStatusPart::slotOpenLink()
{
    QString file_name = KFileDialog::getOpenUrl().url();

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

    Ui::ConfigSearchDialog search_ui;
    Ui::ConfigResultsDialog results_ui;
    
    QWidget *search_widget = new QWidget;
    QWidget *results_widget = new QWidget;
    
    search_ui.setupUi(search_widget);
    results_ui.setupUi(results_widget);

    dialog->addPage(search_widget, i18n("Check"), "zoom-original");
    dialog->addPage(results_widget, i18n("Results"), "fileview-detailed");
    dialog->addPage(new ConfigIdentificationDialog(), i18n("Identification"),
                    "identity", i18n("Configure the way KLinkstatus reports itself"));

    // FIXME check this connection - mismatched arguments between slot and signal
    connect(dialog, SIGNAL(settingsChanged(const QString&)), tabwidget_, SLOT(slotLoadSettings()));
    dialog->show();
}

void KLinkStatusPart::slotAbout()
{
    if(m_dlgAbout == 0)
    {
        static KAboutData * about = createAboutData();
        m_dlgAbout = new KAboutApplicationDialog(about, tabwidget_);
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
