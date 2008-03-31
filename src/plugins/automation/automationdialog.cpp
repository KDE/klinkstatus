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

#include "automationconfig.h"


AutomationDialog::AutomationDialog(QWidget* parent, const QString& name, KConfigSkeleton* configSkeleton)
    : KConfigDialog(parent, name, configSkeleton)
{
    setFaceType(KPageDialog::List);

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
        
        addPage(new QWidget(this), name);
    }
}

AutomationDialog::~AutomationDialog()
{
}

QStringList AutomationDialog::configurationFiles() 
{
    return KGlobal::dirs()->findAllResources("appdata", "automation/*.properties");
}

#include "automationdialog.moc"
