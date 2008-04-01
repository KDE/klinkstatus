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

#include "automationconfig.h"
#include "ui_automationdialogui.h"


class AutomationDialog::AutomationDialogPrivate
{
public:
    AutomationDialogPrivate() {
    }
    
    ~AutomationDialogPrivate() {
    }

    QMap<QWidget*, KCoreConfigSkeleton*> configForPage;
};

AutomationDialog::AutomationDialog(QWidget* parent, const QString& name, KConfigSkeleton* configSkeleton)
    : KConfigDialog(parent, name, configSkeleton), d(new AutomationDialogPrivate())
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

AutomationDialog::~AutomationDialog()
{
    delete d;
}

void AutomationDialog::loadPages()
{
    QStringList configurationFiles = AutomationDialog::configurationFiles();
    kDebug(23100) << configurationFiles;
    
    AutomationConfig::instance(QString());
    
    foreach(QString file, configurationFiles) {
        kDebug(23100) << "Adding site configuration: " << file;
        
        delete AutomationConfig::self();
        AutomationConfig::instance(file);
        AutomationConfig* config = AutomationConfig::self();

        QString name = config->name();
        if(name.isEmpty()) {
            continue;
        }

        Ui::AutomationWidgetUi ui;
        QWidget* widget = new QWidget(this);
        ui.setupUi(widget);

        KPageWidgetItem* pageItem = addPage(widget, config, name);
        d->configForPage.insert(pageItem->widget(), config);
    }
}

QStringList AutomationDialog::configurationFiles()
{
    return KGlobal::dirs()->findAllResources("appdata", "automation/*.properties");
}

void AutomationDialog::slotNewClicked()
{
    
}

void AutomationDialog::slotRemoveClicked()
{
    QString configFilename = d->configForPage[currentPage()->widget()]->config()->name();
        
    if(!QFile(configFilename).remove()) {
        KMessageBox::sorry(this, i18n("Could not delete configuration file %1").arg(configFilename));
        return;
    }
    
    d->configForPage.remove(currentPage()->widget());
    removePage(currentPage());
}


#include "automationdialog.moc"
