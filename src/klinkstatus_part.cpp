/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   pmg@netcabo.pt                                                        *
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

#include "klinkstatus_part.h"
#include "ui/tabwidgetsession.h"
#include "ui/sessionwidget.h"


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

    m_tabwidget = new TabWidgetSession( parentWidget, widgetName );
    setWidget(m_tabwidget);

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

    action_new_link_check_ = new KAction(i18n("New Link Check"),
                                         "filenew",
                                         0, this, SLOT(slotNewLinkCheck()),
                                         actionCollection(), "new_link_check");

    action_open_link_ = new KAction(i18n("Open URL"),
                                    "fileopen",
                                    0, this, SLOT(slotOpenLink()),
                                    actionCollection(), "open_link");

    action_close_tab_ = new KAction(i18n("Close Tab"),
                                    "fileclose",
                                    0, this, SLOT(slotClose()),
                                    actionCollection(), "close_tab");
    action_close_tab_->setEnabled(false);

    (void) new KAction(i18n("About KLinkStatus"),
                       "klinkstatus",
                       0, this, SLOT(slotAbout()),
                       actionCollection(), "about_klinkstatus");
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

    if(m_tabwidget->count() == 0 || !m_tabwidget->emptySessionsExist() )
    {
        m_tabwidget->newSession(url);
    }
    else
    {
        m_tabwidget->getEmptySession()->setUrl(url);
    }

    action_close_tab_->setEnabled(( m_tabwidget->count()>1 ));

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
    QString file_name = KFileDialog::getOpenFileName();

    if (file_name.isEmpty() == false)
    {
        openURL(file_name);
    }
}

void KLinkStatusPart::slotClose()
{
    m_tabwidget->closeSession();

    if(m_tabwidget->count() > 1)
		assert(action_close_tab_->isEnabled());
    else
		action_close_tab_->setEnabled(false);
}

void KLinkStatusPart::slotAbout()
{
    if(m_dlgAbout == 0)
    {
        m_dlgAbout = new KAboutApplication(createAboutData(), m_tabwidget, "about_app");
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

/*
void KLinkStatusPart::fileNew()
{
    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked

    // About this function, the style guide (
    // http://developer.kde.org/documentation/standards/kde/style/basics/index.html )
    // says that it should open a new window if the document is _not_
    // in its initial state.  This is what we do here.. -> i don't think so
    openURL(KURL());

    if(m_tabwidget->count() > 1)
        actionCollection()->action(KStdAction::stdName(KStdAction::Close))->setEnabled(true);
    else
        actionCollection()->action(KStdAction::stdName(KStdAction::Close))->setEnabled(false);
}

void KLinkStatusPart::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    QString file_name = KFileDialog::getOpenFileName();

    if (file_name.isEmpty() == false)
    {
        openURL(file_name);
    }
}

void KLinkStatusPart::fileClose()
{
    m_tabwidget->closeSession();

    if(m_tabwidget->count() > 1)
        actionCollection()->action(KStdAction::stdName(KStdAction::Close))->setEnabled(true);
    else
        actionCollection()->action(KStdAction::stdName(KStdAction::Close))->setEnabled(false);
}
*/
KAboutData* KLinkStatusPart::createAboutData()
{
    KAboutData * about = new KAboutData("klinkstatuspart", I18N_NOOP("KLinkStatus"), "0.1.0");

    about->addAuthor("Paulo Moura Guedes", 0, "pmg@netcabo.pt");

    about->addCredit("Andras Mantia", 0, "amantia@kde.org");
    about->addCredit("Eric Laffoon", 0, "sequitur@kde.org");
    about->addCredit("Gonçalo Silva", 0, "gngs@paradigma.co.pt");
    about->addCredit("Mathieu Kooiman", 0, " quanta@map-is.nl");
    about->addCredit("Manuel Menezes de Sequeira", 0, 0, "http://home.iscte.pt/~mms/");
    about->addCredit("Michal Rudolf", 0, "mrudolf@kdewebdev.org");
    about->addCredit("Gonçalo Silva", 0, "gngs@paradigma.co.pt");
    about->addCredit("Nuno Monteiro", 0, 0, "http://www.itsari.org");

    return about;
}

#include "klinkstatus_part.moc"
