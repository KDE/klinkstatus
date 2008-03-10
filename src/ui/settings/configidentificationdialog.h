/***************************************************************************
 *   Copyright (C) 2006 by Paulo Moura Guedes                              *
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

#ifndef CONFIGIDENTIFICATIONDIALOG_H
#define CONFIGIDENTIFICATIONDIALOG_H

#include "ui_configidentificationdialogui.h"

#include "klinkstatus_export.h"


/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class KLINKSTATUS_EXPORT ConfigIdentificationDialog : 
        public QWidget, public Ui::ConfigIdentificationDialogUi
{

Q_OBJECT

public:
    ConfigIdentificationDialog(QWidget *parent = 0);
    ~ConfigIdentificationDialog();

private slots:
    void slotDefaultUA();

};

#endif
