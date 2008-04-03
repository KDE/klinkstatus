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


PimConfigDialog::PimConfigDialog(QWidget* parent, Qt::WFlags flags)
  : KPageDialog(parent, flags)
{
    setFaceType(KPageDialog::List);
    setCaption(i18n("Configure PIM information"));
    setInitialSize(QSize(555,280));
    
    QWidget* mailTransportPage = new MailTransportWidget(this);
    KPageWidgetItem* mailTransportItem = addPage(mailTransportPage, i18n("Mail Transport"));
    mailTransportItem->setHeader(i18n("Mail Transport"));
    mailTransportItem->setIcon(KIcon("configure"));
}

PimConfigDialog::~PimConfigDialog()
{
}


MailTransportWidget::MailTransportWidget(QWidget* parent)
  : QWidget(parent)
{
    KCModule* module = KCModuleLoader::loadModule("kcm_mailtransport", KCModuleLoader::Inline, this);
}

MailTransportWidget::~MailTransportWidget()
{
}


#include "pimconfigdialog.moc"
