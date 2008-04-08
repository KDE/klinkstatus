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

#include "pimconfigdialog.h"

#include <KLocale>
#include <KCModuleLoader>

#include <kpimidentities/identity.h>
#include <kpimidentities/identitymanager.h>
#include <mailtransport/transport.h>
#include <mailtransport/transportmanager.h>

#include "klsconfig.h"


PimConfigDialog::PimConfigDialog(QWidget* parent, const QString &name, KConfigSkeleton* configSkeleton)
  : KConfigDialog(parent, name, configSkeleton),
    m_showIdentityPage(true),
    m_showMailTransportPage(true)
{
//     setButtons(Default|Ok|Apply);
//     enableButtonOk(false);
    
//     connect(this, SIGNAL(settingsChanged(const QString&)), this, SLOT(slotSettingsChanged()));
    
    setFaceType(KPageDialog::List);
    setCaption(i18n("Configure PIM information"));
    setInitialSize(QSize(555,280));
    
    if(m_showMailTransportPage) {
        QWidget* mailTransportPage = new MailTransportWidget(this);
        KPageWidgetItem* mailTransportItem = addPage(mailTransportPage, i18n("Mail Transport"));
        mailTransportItem->setHeader(i18n("Mail Transport"));
        mailTransportItem->setIcon(KIcon("configure"));
    }
    
    if(m_showIdentityPage) {
        QWidget* identityPage = new IdentityWidget(this);
        KPageWidgetItem* identityItem = addPage(identityPage, i18n("User Information"));
        identityItem->setHeader(i18n("User Information"));
        identityItem->setIcon(KIcon("configure"));
    }
}

PimConfigDialog::~PimConfigDialog()
{
}

void PimConfigDialog::slotSettingsChanged(QString const&)
{
}


IdentityWidget::IdentityWidget(QWidget* parent)
  : QWidget(parent)
{
    setupUi(this);
    
    connect(kcfg_UseSystemIdentity, SIGNAL(stateChanged(int)), this, SLOT(slotUseSystemStateChanged(int)));
    
    KPIMIdentities::IdentityManager identityManager(false, 0, "IdentityManager");
    KPIMIdentities::Identity const& identity = identityManager.defaultIdentity();

    if(identity == KPIMIdentities::Identity::null()) {
        kcfg_UseSystemIdentity->setEnabled(false);
    }
    else {
        QString name = identity.fullName();
        QString email = identity.emailAddr();

        if(name.isEmpty() || email.isEmpty()) {
            kcfg_UseSystemIdentity->setEnabled(false);
        }
        else {
            kcfg_UseSystemIdentity->setEnabled(true);
        }
    }
}

IdentityWidget::~IdentityWidget()
{
}

void IdentityWidget::slotUseSystemStateChanged(int state)
{
    kDebug() << "IdentityWidget::slotUseSystemStateChanged - state: " << state;
  
    bool enable = (state == Qt::Unchecked);
    
    formLayout->setEnabled(enable);
    
    KLSConfig::setUseSystemIdentity(enable);
}

MailTransportWidget::MailTransportWidget(QWidget* parent)
  : QWidget(parent)
{
    KCModuleLoader::loadModule("kcm_mailtransport", KCModuleLoader::Inline, this);
}

MailTransportWidget::~MailTransportWidget()
{
}


#include "pimconfigdialog.moc"
