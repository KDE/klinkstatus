/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "automationpart.h"

#include <kpimidentities/identity.h>
#include <kpimidentities/identitymanager.h>
#include <mailtransport/transport.h>
#include <mailtransport/transportmanager.h>

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kcmdlineargs.h>
#include <kurl.h>
#include <klocale.h>
#include <kconfiggroup.h>

#include "automationdialog.h"
#include "automationconfig.h"
#include "engine/searchmanageragent.h"
#include "utils/timer.h"
#include "pim/pimconfigdialog.h"
#include "klsconfig.h"


typedef KGenericFactory<AutomationPart> KLinkStatusAutomationFactory;
K_EXPORT_COMPONENT_FACTORY(automationklinkstatus, KLinkStatusAutomationFactory("automationklinkstatus"))

class AutomationPart::Private
{
public:
    Private() :
        automationDialog(0)
    {}
    
    ~Private() {
        delete automationDialog;
    }
    
    QStringList configurationFiles;
    AutomationDialog* automationDialog;
};

AutomationPart::AutomationPart(QObject* parent, const QStringList&)
    : KParts::Plugin(parent)
    , d(new Private())
{
    setComponentData(AutomationPart::componentData());
    setXMLFile(KStandardDirs::locate("data", "klinkstatus/kpartplugins/klinkstatus_automation.rc"), true);
    kDebug(23100) <<"Automation plugin. Class:" << metaObject()->className()
        <<", Parent:" << parent->metaObject()->className();
        
    initActions();
    initLinkChecks();
}

AutomationPart::~AutomationPart()
{
    delete d;
}

void AutomationPart::initActions()
{
    KAction* action  = new KAction(i18n("Schedule Link Checks..."), this);
    actionCollection()->addAction("schedule_checks", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotConfigureLinkChecks()));
}

void AutomationPart::initLinkChecks()
{
    kDebug(23100) << "AutomationPart::initLinkChecks";

    d->configurationFiles = KGlobal::dirs()->findAllResources("appdata", "automation/*.properties");
    
    foreach(const QString &file, d->configurationFiles) {
        scheduleCheck(file);
    }
}

void AutomationPart::scheduleCheck(QString const& configurationFile)
{
    kDebug(23100) << "configurationFile: " << configurationFile;
    
    AutomationConfig config(KSharedConfig::openConfig(configurationFile));

    int periodicity = config.periodicity();
    QString hour(config.hour());

    kDebug(23100) << "periodicity: " << periodicity;
    kDebug(23100) << "hour: " << hour;
    
    if(hour.isEmpty()) {
        kWarning(23100) << "hour.isEmpty()";
        return;
    }
    
    QTime time = QTime::fromString(hour, "hh:mm");
    int interval = -1;
    // FIXME Magic numbers!!!!!!!!
    // hourly
    if(periodicity == 0) {
        interval = 1 * 60 * 60 * 1000;
    }
    // daily
    else if(periodicity == 1) {
        interval = 1 * 24 * 60 * 60 * 1000;
    }
    // weekly
    else if(periodicity == 2) {
        interval = 7 * 24 * 60 * 60 * 1000;
    }
    
    kDebug(23100) << "interval: " << interval;
    kDebug(23100) << "time: " << time;
    
    if(interval <= 0 || !time.isValid()) {
        kWarning(23100) << "interval <= 0 || !time.isValid()";
        return;
    }
    
    SearchManagerAgent* agent = chooseSearchManagerAgent(configurationFile);
    agent->setOptionsFilePath(configurationFile);
    
    Timer* timer = new Timer(agent, this);
    timer->start(time, interval);
    connect(timer, SIGNAL(timeout(QObject*)), this, SLOT(slotTimeout(QObject*)));
}

SearchManagerAgent* AutomationPart::chooseSearchManagerAgent(QString const& configurationFile)
{
    QList<SearchManagerAgent*> agents = findChildren<SearchManagerAgent*> ();
    
    foreach(SearchManagerAgent* agent, agents) {
        if(agent->optionsFilePath() == configurationFile) {
            return agent;
        }
    }
    
    return new SearchManagerAgent(this);
}

void AutomationPart::slotConfigureLinkChecks()
{
    delete d->automationDialog;
    
    AutomationConfig* config = new AutomationConfig(KSharedConfig::openConfig());
    d->automationDialog = new AutomationDialog(0, "automationDialog", config);
    connect(d->automationDialog, SIGNAL(settingsChanged(const QString&)),
            this, SLOT(slotAutomationSettingsChanged(const QString&)));
    connect(d->automationDialog, SIGNAL(finished()),
            this, SLOT(slotAutomationSettingsFinished()));
    
    d->automationDialog->show();
}

void AutomationPart::slotTimeout(QObject* delegate)
{
    kDebug(23100) << "AutomationPart::slotTimeout";
    
    SearchManagerAgent* agent = static_cast<SearchManagerAgent*> (delegate);
    agent->check();
}

void AutomationPart::slotAutomationSettingsChanged(QString const&)
{
    kDebug(23100) << "AutomationPart::slotAutomationSettingsChanged";
    
    QList<Timer*> timers = findChildren<Timer*> ();
    
    foreach(Timer* timer, timers) {
        timer->stop();
        timer->setParent(0);
        timer->deleteLater();
    }
    
    initLinkChecks();
}

void AutomationPart::slotAutomationSettingsFinished()
{
    bool needIdentity = false;
    bool needMailTransport = false;

//     KPIMIdentities::IdentityManager identityManager(false, 0, "IdentityManager");
//     KPIMIdentities::Identity const& identity = identityManager.defaultIdentity();
//     
//     if(identity == KPIMIdentities::Identity::null()) {
//         needIdentity = true;
//     }
//     else {
//         QString name = identity.fullName();
//         QString email = identity.emailAddr();
//         
//         if(name.isEmpty() || email.isEmpty()) {
//             QString appUserName = KLSConfig::userName();
//             QString appUserEmail = KLSConfig::userEmail();
//         
//             if(appUserName.isEmpty() || appUserEmail.isEmpty()) {
//                 needIdentity = true;
//             }
//             else {
//                 KPIMIdentities::Identity& ident = 
//                         identityManager.modifyIdentityForName(identity.identityName());
//                 if(name.isEmpty()) {
//                     ident.setFullName(appUserName);
//                 }
//                 if(email.isEmpty()) {
//                     ident.setEmailAddr(appUserEmail);
//                 }
//                 
//                 identityManager.commit();
//             }
//         }
//         else {
//             kDebug() << "KPIMIdentities::Identity is well configured!";
//         }
//     }
    
    QString defaultTransport = MailTransport::TransportManager::self()->defaultTransportName();
    if(defaultTransport.isEmpty()) {
        needMailTransport = true;
    }
    
    if(needIdentity || needMailTransport) {
        PimConfigDialog dialog(0, "pimConfigDialog", KLSConfig::self());
        dialog.exec();
    }
}


#include "automationpart.h"
