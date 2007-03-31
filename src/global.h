/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
class QLabel;
#include <QTimer>

namespace KParts {
    class ReadOnlyPart;
    class StatusBarExtension;
}
class KStatusBar;

using namespace KParts;
    
/**
@author Paulo Moura Guedes
*/
class Global : public QObject
{
    Q_OBJECT
public:
    static Global* self();
    ~Global();
    
    void setKLinkStatusPart(ReadOnlyPart* part);
    KStatusBar* statusBar() const;
//     static ReadOnlyPart* getKLinkStatusPart();
    
//     static StatusBarExtension* getStatusBarExtension();

    void setStatusBarText(QString const& text, bool permanent = false);
    void addStatusBarPermanentItem(QWidget* widget);

private slots:
    void slotRemoveStatusBarLabel();
    void slotStatusBarTimeout();
    
private:
    Global(QObject *parent = 0);

private:
    static Global* m_self_;
    static ReadOnlyPart* m_klinkStatusPart;
    static StatusBarExtension* m_statusBarExtension;
    QLabel* m_statusBarLabel;
    // This timer is a workaround for cleaning the temporary messages of tree items (statusTip)
    // which sometimes don't get hidden
    QTimer m_statusBarTimer;
};

#endif
