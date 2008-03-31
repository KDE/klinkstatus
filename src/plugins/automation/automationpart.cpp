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
    
    foreach(QString file, d->configurationFiles) {
        scheduleCheck(file);
    }
}

void AutomationPart::scheduleCheck(QString const& configurationFile)
{
    kDebug(23100) << "configurationFile: " << configurationFile;
    
    AutomationConfig::instance(QString());
    delete AutomationConfig::self();
    AutomationConfig::instance(configurationFile);
    AutomationConfig* config = AutomationConfig::self();
    
    QString periodicity(config->periodicity());
    QString hour(config->hour());

    if(periodicity.isEmpty() || hour.isEmpty()) {
        kDebug(23100) << "periodicity.isEmpty() || hour.isEmpty()";
        return;
    }
    
    kDebug(23100) << "periodicity: " << periodicity;
    kDebug(23100) << "hour: " << hour;
    
    QTime time = QTime::fromString(hour, "hh:mm");
    int interval = -1;
    if(periodicity == "hourly") {
        interval = 1 * 60 * 60 * 1000;
    }
    else if(periodicity == "daily") {
        interval = 1 * 24 * 60 * 60 * 1000;
    }
    else if(periodicity == "weekly") {
        interval = 7 * 24 * 60 * 60 * 1000;
    }
    
    kDebug(23100) << "interval: " << interval;
    kDebug(23100) << "time: " << time;
    
    SearchManagerAgent* agent = new SearchManagerAgent(this);
    agent->setOptionsFilePath(configurationFile);
    
    Timer* timer = new Timer(agent, this);
    timer->start(time, interval);
    connect(timer, SIGNAL(timeout(QObject*)), this, SLOT(slotTimeout(QObject*)));
}

void AutomationPart::slotConfigureLinkChecks()
{
    AutomationConfig::instance(QString());
    delete AutomationConfig::self();
    AutomationConfig::instance(QString());

    delete d->automationDialog;
    d->automationDialog = new AutomationDialog(0, "automationDialog", AutomationConfig::self());
//     connect(dialog, SIGNAL(finished(int)), this, SLOT(slotAutomationDialogFinished(int)));
    
    d->automationDialog->show();
}

void AutomationPart::slotTimeout(QObject* delegate)
{
    kDebug(23100) << "AutomationPart::slotTimeout";
    
    SearchManagerAgent* agent = static_cast<SearchManagerAgent*> (delegate);
    agent->check();
}

void AutomationPart::slotAutomationDialogFinished(int result)
{
    kDebug(23100) << "AutomationPart::slotTimeout - result: " << result;
}


#include "automationpart.h"
