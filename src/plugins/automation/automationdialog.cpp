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

#include "automationdialog.h"

#include <KStandardDirs>
#include <KPageDialog>
#include <KDebug>
#include <KConfig>
#include <KMessageBox>

#include <QMap>

#include "automationconfigpage.h"
#include "automationconfig.h"
#include "ui_automationconfigpageui.h"


class AutomationDialog::AutomationDialogPrivate
{
public:
    AutomationDialogPrivate(KConfigSkeleton* configSkeleton) 
        : configSkeleton(configSkeleton)
    {
    }
    
    ~AutomationDialogPrivate() {
        delete configSkeleton;
        
        foreach(KCoreConfigSkeleton* config, configForPage) {
            delete config;
            config = 0;
        }
    }

    KConfigSkeleton* configSkeleton;
    QMap<QWidget*, KCoreConfigSkeleton*> configForPage;
};

AutomationDialog::AutomationDialog(QWidget* parent, const QString& name, KConfigSkeleton* configSkeleton)
    : KConfigDialog(parent, name, configSkeleton), d(new AutomationDialogPrivate(configSkeleton))
{
    setFaceType(KPageDialog::List);
    setCaption(i18n("Configure Site check Automation"));
    
    setButtons(Default|Ok|Apply|Cancel|User1|User2);
    setButtonText(User1, "New...");
    setButtonIcon(User1, KIcon());
    setButtonText(User2, "Remove");
    setButtonIcon(User2, KIcon());
    
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotNewClicked()));
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotRemoveClicked()));
    
    loadPages();
}

// void slotCurrentPageChanged (KPageWidgetItem* current, KPageWidgetItem* before)
// {
//     KCoreConfigSkeleton* config = d->configForPage[current->widget()];
// }

AutomationDialog::~AutomationDialog()
{
    delete d;
}

void AutomationDialog::loadPages()
{
    QStringList configurationFiles = AutomationDialog::configurationFiles();
    kDebug(23100) << configurationFiles;
    
    foreach(QString file, configurationFiles) {
        kDebug(23100) << "Adding site configuration: " << file;
        
        AutomationConfig* config = new AutomationConfig(KSharedConfig::openConfig(file));

        QString name = config->name();
        if(name.isEmpty()) {
            continue;
        }

//         Ui::AutomationWidgetUi ui;
//         QWidget* widget = new QWidget(this);
//         ui.setupUi(widget);

        AutomationConfigPage* page = new AutomationConfigPage(config, this);

        KPageWidgetItem* pageItem = addPage(page, config, name);
        d->configForPage.insert(pageItem->widget(), config);
    }
}

QStringList AutomationDialog::configurationFiles()
{
    return KGlobal::dirs()->findAllResources("appdata", "automation/*.properties");
}

void AutomationDialog::slotNewClicked()
{
    NewScheduleAssistant assistant(this);    
    assistant.exec();
}

void AutomationDialog::slotRemoveClicked()
{
    QString configFilename = d->configForPage[currentPage()->widget()]->config()->name();
        
    QFile file(configFilename);
    if(file.exists() && !file.remove()) {
        KMessageBox::sorry(this, i18n("Could not delete configuration file %1").arg(configFilename));
        return;
    }
    
    d->configForPage.remove(currentPage()->widget());
    removePage(currentPage());
}


NewScheduleAssistant::NewScheduleAssistant(AutomationDialog* parent, Qt::WFlags flags)
    : KAssistantDialog(parent, flags), m_parent(parent), m_lineEdit(0)
{
    QWidget* widget = new QWidget(this);
    m_lineEdit = new KLineEdit(widget);
    m_lineEdit->setMinimumWidth(300);

    KPageWidgetItem* page = addPage(widget, "Recurring Check Name");
    setValid(page, false);

    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotFinishClicked()));

    connect(m_lineEdit, SIGNAL(textChanged(const QString&)),
    this, SLOT(slotNameChanged(const QString&)));
}

NewScheduleAssistant::~NewScheduleAssistant() {}

QString NewScheduleAssistant::scheduleName() const {
    return m_lineEdit->text();
}

void NewScheduleAssistant::slotNameChanged(const QString& text) {
    setValid(currentPage(), !text.isEmpty());
}

void NewScheduleAssistant::slotFinishClicked()
{
    QString file = KGlobal::dirs()->saveLocation("data") 
            + "klinkstatus/automation/" + scheduleName() + ".properties";
    kDebug(23100) << "Adding site configuration: " << file;

    AutomationConfig* config = new AutomationConfig(KSharedConfig::openConfig(file));

    config->setName(scheduleName());

//     Ui::AutomationWidgetUi ui;
//     QWidget* widget = new QWidget(this);
//     ui.setupUi(widget);

    AutomationConfigPage* page = new AutomationConfigPage(config, m_parent);

    KPageWidgetItem* pageItem = m_parent->addPage(page, config, scheduleName());
    m_parent->setCurrentPage(pageItem);
    
    m_parent->d->configForPage.insert(pageItem->widget(), config);
}


#include "automationdialog.moc"
