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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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

#include "klinkstatus_part.h"
#include "ui/tabwidgetsession.h"
#include "ui/sessionwidget.h"


const char KLinkStatusPart::description_[] = "A Link Checker";
const char KLinkStatusPart::version_[] = "0.1.0-CVS";

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

    // File menu

    action_new_link_check_ = new KAction(i18n("New Link Check"), "filenew",
                                         0, this, SLOT(slotNewLinkCheck()),
                                         actionCollection(), "new_link_check");

    action_open_link_ = new KAction(i18n("Open URL..."), "fileopen",
                                    0, this, SLOT(slotOpenLink()),
                                    actionCollection(), "open_link");

    action_close_tab_ = new KAction(i18n("Close Tab"), "fileclose",
                                    0, this, SLOT(slotClose()),
                                    actionCollection(), "close_tab");
    action_close_tab_->setEnabled(false);

    // View menu

    action_display_all_links_ = new KAction(i18n("All links"), "",
                                            0, this, SLOT(slotDisplayAllLinks()),
                                            actionCollection(), "display_all_links");
    action_display_all_links_->setEnabled(false);

    action_display_good_links_ = new KAction(i18n("Good links"), "ok",
                                 0, this, SLOT(slotDisplayGoodLinks()),
                                 actionCollection(), "display_good_links");
    action_display_good_links_->setEnabled(false);

    action_display_bad_links_ = new KAction(i18n("Broken links"), "no",
                                            0, this, SLOT(slotDisplayBadLinks()),
                                            actionCollection(), "display_bad_links");
    action_display_bad_links_->setEnabled(false);

    action_display_malformed_links_ = new KAction(i18n("Malformed links"),
                                      "bug",
                                      0, this, SLOT(slotDisplayMalformedLinks()),
                                      actionCollection(), "display_malformed_links");
    action_display_malformed_links_->setEnabled(false);

    action_display_undetermined_links_ = new KAction(i18n("Undetermined links"), "help",
                                         0, this, SLOT(slotDisplayUndeterminedLinks()),
                                         actionCollection(), "display_undetermined_links");
    action_display_undetermined_links_->setEnabled(false);

    // Help menu

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
    //emit setWindowCaption( url.prettyURL() );

    if(tabwidget_->count() == 0 || !tabwidget_->emptySessionsExist() )
    {
        connect(tabwidget_->newSession(url), SIGNAL(signalSearchFinnished()),
                this, SLOT(slotEnableDisplayLinksActions()));
    }
    else
    {
        tabwidget_->getEmptySession()->setUrl(url);
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
    action_display_good_links_->setEnabled(true);
    action_display_bad_links_->setEnabled(true);
    action_display_malformed_links_->setEnabled(true);
    action_display_undetermined_links_->setEnabled(true);
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
    KBugReport dlg(widget(), true, createAboutData());
    dlg.exec();
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

    return about;
}

#include "klinkstatus_part.moc"
