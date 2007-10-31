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

#include "global.h"
#include <QtDBus>
#include <QString>
#include <QTimer>
#include <QLabel>
    
#include <kdebug.h>
#include <kapplication.h>
#include <kurl.h>
#include <KStatusBar>
#include <kparts/statusbarextension.h>
#include <kparts/part.h>
    
#include <sys/types.h>
#include <unistd.h>

class GlobalPrivate : public QObject
{
    Q_OBJECT
public:
    GlobalPrivate();

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
    static ReadOnlyPart* m_klinkStatusPart;
    static StatusBarExtension* m_statusBarExtension;
    QLabel* m_statusBarLabel;
    // This timer is a workaround for cleaning the temporary messages of tree items (statusTip)
    // which sometimes don't get hidden
    QTimer m_statusBarTimer;
};

ReadOnlyPart* GlobalPrivate::m_klinkStatusPart = 0;
StatusBarExtension* GlobalPrivate::m_statusBarExtension = 0;

K_GLOBAL_STATIC(GlobalPrivate, global_instance)

GlobalPrivate::GlobalPrivate()
        : QObject(0), m_statusBarLabel(0)
{
    connect(&m_statusBarTimer, SIGNAL(timeout()),
            this, SLOT(slotStatusBarTimeout()));

    m_statusBarTimer.start(1500);
}

void GlobalPrivate::setKLinkStatusPart(ReadOnlyPart* part)
{
    m_klinkStatusPart = part;

    if(part)
        m_statusBarExtension = new StatusBarExtension(part);

    m_statusBarLabel = new QLabel(statusBar());
}


void Global::setKLinkStatusPart(ReadOnlyPart* part)
{
    global_instance->setKLinkStatusPart(part);
}

KStatusBar* GlobalPrivate::statusBar() const
{
    if(!m_statusBarExtension)
        return 0;

    return m_statusBarExtension->statusBar();
}


KStatusBar* Global::statusBar()
{
    return global_instance->statusBar();
}

void GlobalPrivate::setStatusBarText(QString const& text, bool permanent)
{
    if(!m_statusBarExtension)
        return;

    m_statusBarLabel->setText(text);
    m_statusBarExtension->addStatusBarItem(m_statusBarLabel, 0, permanent);

    // This is a hack for removing the added messages from the status bar.
    // Permanent seems to don't have any effect
    if(!permanent)
        QTimer::singleShot(1000 * 3, this, SLOT(slotRemoveStatusBarLabel()));
}


void Global::setStatusBarText(QString const& text, bool permanent)
{
    global_instance->setStatusBarText(text, permanent);
}

void GlobalPrivate::addStatusBarPermanentItem(QWidget* widget)
{
    if(!m_statusBarExtension)
        return;

    m_statusBarExtension->addStatusBarItem(widget, 0, true);
}


void Global::addStatusBarPermanentItem(QWidget* widget)
{
    global_instance->addStatusBarPermanentItem(widget);
}

void GlobalPrivate::slotRemoveStatusBarLabel()
{
    m_statusBarExtension->removeStatusBarItem(m_statusBarLabel);
}


void GlobalPrivate::slotStatusBarTimeout()
{
    m_statusBarExtension->statusBar()->clearMessage();
}


#include "global.moc"
