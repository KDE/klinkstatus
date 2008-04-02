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

#ifndef AUTOMATIONDIALOG_H
#define AUTOMATIONDIALOG_H

#include <KConfigDialog>
#include <KAssistantDialog>
#include <KLineEdit>

#include <QStringList>


/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class AutomationDialog : public KConfigDialog
{
    Q_OBJECT
public:
    friend class NewScheduleAssistant;

    AutomationDialog(QWidget* parent, const QString &name, KConfigSkeleton* config);
    ~AutomationDialog();

private Q_SLOTS:
    void slotNewClicked();
    void slotRemoveClicked();

private:
    static QStringList configurationFiles();
    
    void loadPages();
    
private:
    class AutomationDialogPrivate;
    AutomationDialogPrivate* const d;
};

class NewScheduleAssistant : public KAssistantDialog
{
    Q_OBJECT
public:
    NewScheduleAssistant(AutomationDialog* parent = 0, Qt::WFlags flags = 0);
    ~NewScheduleAssistant();
    
    QString scheduleName() const;
    
private Q_SLOTS:
    void slotNameChanged(const QString& text);
    void slotFinishClicked();

private:
    AutomationDialog* m_parent;
    KLineEdit* m_lineEdit;
};



#endif
