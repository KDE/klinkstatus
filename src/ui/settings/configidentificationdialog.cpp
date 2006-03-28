//
// C++ Implementation: configidentificationdialog
//
// Description: 
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "configidentificationdialog.h"

#include <kprotocolmanager.h>
#include <kpushbutton.h>
#include <klineedit.h>

#include <qstring.h>

#include "../cfg/klsconfig.h"


ConfigIdentificationDialog::ConfigIdentificationDialog(QWidget *parent, const char *name)
 : ConfigIdentificationDialogUi(parent, name)
{
    if(KLSConfig::userAgent().isEmpty())
    {
        slotDefaultUA();
    }
    
    connect(buttonDefault, SIGNAL(clicked()), this, SLOT(slotDefaultUA()));
}


ConfigIdentificationDialog::~ConfigIdentificationDialog()
{
}

void ConfigIdentificationDialog::slotDefaultUA()
{
    KLSConfig::setUserAgent(KProtocolManager::defaultUserAgent());
    kcfg_UserAgent->setText(KLSConfig::userAgent());
}


#include "configidentificationdialog.moc"
