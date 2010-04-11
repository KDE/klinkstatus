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

#include "configresultsdialogimpl.h"

#include "klsconfig.h"


ConfigResultsDialogImpl::ConfigResultsDialogImpl(QWidget *parent) 
    : QWidget(parent)
{
    setupUi(this);

    initStylesCombo();
}

ConfigResultsDialogImpl::~ConfigResultsDialogImpl()
{
}

void ConfigResultsDialogImpl::initStylesCombo()
{
    QStringList availableStyles = KLSConfig::stylesheetFiles();
    foreach(const QString &style, availableStyles) {
        kcfg_PreferedStylesheet->addItem(style);
    }
    
    QString preferred = KLSConfig::preferedStylesheet();
    int i = kcfg_PreferedStylesheet->findText(preferred);
    if(i != -1) {
        kcfg_PreferedStylesheet->setCurrentIndex(i);
    }
    else {
        kcfg_PreferedStylesheet->insertItem(0, preferred);
        kcfg_PreferedStylesheet->setCurrentItem(0);
    }
}


#include "configresultsdialogimpl.moc"

