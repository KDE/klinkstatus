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

#ifndef PIMCONFIGDIALOG_H
#define PIMCONFIGDIALOG_H

#include <KConfigDialog>

#include "klinkstatus_export.h"
#include "ui_identitywidgetui.h"

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class KLINKSTATUS_EXPORT PimConfigDialog : public KConfigDialog
{
    Q_OBJECT
public:
    PimConfigDialog(QWidget* parent, const QString &name, KConfigSkeleton* config);
    ~PimConfigDialog();
    
//     void setShowIdentityPage(bool show) { m_showIdentityPage = show; }
//     void setShowMailTransportPage(bool show) { m_showMailTransportPage = show; }
    
private Q_SLOTS:
    void slotSettingsChanged(QString const&);

private:
    bool m_showIdentityPage;
    bool m_showMailTransportPage;
};


class KLINKSTATUS_EXPORT IdentityWidget : public QWidget, public Ui::IdentityWidgetUi
{
    Q_OBJECT
public:
    IdentityWidget(QWidget* parent = 0);
    ~IdentityWidget();
    
private Q_SLOTS:
    void slotUseSystemStateChanged(int state);
};

class KLINKSTATUS_EXPORT MailTransportWidget : public QWidget
{
    Q_OBJECT
public:
    MailTransportWidget(QWidget* parent = 0);
    ~MailTransportWidget();
};


#endif
