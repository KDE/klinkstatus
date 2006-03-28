//
// C++ Interface: configidentificationdialog
//
// Description: 
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONFIGIDENTIFICATIONDIALOG_H
#define CONFIGIDENTIFICATIONDIALOG_H

#include "configidentificationdialogui.h"

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class ConfigIdentificationDialog : public ConfigIdentificationDialogUi
{
Q_OBJECT
public:
    ConfigIdentificationDialog(QWidget *parent = 0, const char *name = 0);
    ~ConfigIdentificationDialog();
    
private slots:
    void slotDefaultUA();
};

#endif
