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

#include "automationconfigpage.h"

#include "automationconfig.h"


AutomationConfigPage::AutomationConfigPage(AutomationConfig* config, QWidget *parent)
    : QWidget(parent), m_config(config)
{
    setupUi(this);
    
    initComponents();
}

AutomationConfigPage::~AutomationConfigPage()
{
}

void AutomationConfigPage::initComponents()
{
    kcfg_ResultsFilePath->setMode(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
    kcfg_DocumentRoot->setMode(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
    
    kcfg_Periodicity->insertItem(0, i18n ("Hourly") );
    kcfg_Periodicity->insertItem(1, i18n ("Daily") );
    kcfg_Periodicity->insertItem(2, i18n ("Weekly") );
    
    kDebug(23100) << m_config->periodicity();
    kcfg_Periodicity->setCurrentIndex(m_config->periodicity());
}


#include "automationconfigpage.moc"
