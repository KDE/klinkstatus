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
#include <KStandardDirs>
#include <kparts/statusbarextension.h>
#include <kparts/part.h>

#include <sys/types.h>
#include <unistd.h>

#include "utils/utils.h"


class Global::GlobalPrivate : public QObject
{
    Q_OBJECT
public:
    friend class Global;
  
    GlobalPrivate();

    void setKLinkStatusPart(ReadOnlyPart* part);
    KStatusBar* statusBar() const;

    void setStatusBarText(QString const& text, bool permanent = false);
    void addStatusBarPermanentItem(QWidget* widget);

private slots:
    void slotRemoveStatusBarLabel();
    void slotStatusBarTimeout();
    
private:
    ReadOnlyPart* m_klinkStatusPart;
    bool m_partDestroyed;
    StatusBarExtension* m_statusBarExtension;
    QLabel* m_statusBarLabel;
    // This timer is a workaround for cleaning the temporary messages of tree items (statusTip)
    // which sometimes don't get hidden
    QTimer m_statusBarTimer;

    QDomDocument m_sessionsDocument;
};

Global::GlobalPrivate::GlobalPrivate()
    : QObject(0), m_klinkStatusPart(0), m_partDestroyed(false), m_statusBarExtension(0), m_statusBarLabel(0)
{
    connect(&m_statusBarTimer, SIGNAL(timeout()),
            this, SLOT(slotStatusBarTimeout()));

    m_statusBarTimer.start(1500);
}

void Global::GlobalPrivate::setKLinkStatusPart(ReadOnlyPart* part)
{
    m_klinkStatusPart = part;

    if(part)
        m_statusBarExtension = new StatusBarExtension(part);

    m_statusBarLabel = new QLabel(statusBar());
}


K_GLOBAL_STATIC(Global, globalInstance)

Global* Global::getInstance()
{
    return globalInstance;
}

Global::Global()
    : global_private(new GlobalPrivate())
{
}

Global::~Global()
{
    delete global_private;
}

void Global::setKLinkStatusPart(ReadOnlyPart* part)
{
    global_private->setKLinkStatusPart(part);
}

void Global::setPartDestroyed()
{
    global_private->m_partDestroyed = true;
}


KStatusBar* Global::GlobalPrivate::statusBar() const
{
    if(!m_statusBarExtension)
        return 0;

    return m_statusBarExtension->statusBar();
}


KStatusBar* Global::statusBar() const
{
    return global_private->statusBar();
}

QDomDocument& Global::sessionsDocument()
{
    QDomDocument& doc = global_private->m_sessionsDocument;
    
    if(!doc.isNull()) {
        return doc;
    }

    QString urlString = KStandardDirs::locateLocal("appdata", "sessions.xml");
    kDebug(23100) << urlString;

    QString fileContent = FileManager::read(urlString);
    
    if(!fileContent.isNull() && !fileContent.isEmpty())
    {
        QFile file(urlString);
        if(file.open(QIODevice::ReadOnly))
        {
            if(doc.setContent(&file)) {
                file.close();
                return doc;
            }
        }
        file.close();
    }
     
    Q_ASSERT(doc.isNull());
    
    doc.appendChild(doc.createProcessingInstruction( "xml",
                                       "version=\"1.0\" encoding=\"UTF-8\""));
    
    QDomElement root = doc.createElement("sessions");
    doc.appendChild(root);

    saveSessionsDocument();

    return doc;
}

void Global::findCurrentSession(QString const& url, QDomElement& element)
{
    QDomDocument const doc = sessionsDocument();

    QDomElement sessions = doc.namedItem("sessions").toElement();
    QDomNode n = sessions.firstChild();
    while(!n.isNull()) {
        if(n.isElement()) {
            QDomElement e = n.toElement();
            if(e.tagName() == "session" && e.attribute("url").toLower() == url.toLower())
            {
//                 kDebug(23100) << "Session found: " << url;
                element = e;
                return;
            }
        }
        n = n.nextSibling();
    }
}

void Global::saveSessionsDocument() const
{
    QDomDocument& doc = global_private->m_sessionsDocument;
    QString urlString = KStandardDirs::locateLocal("appdata", "sessions.xml");
    kDebug(23100) << urlString;

    FileManager::write(doc.toString(4), KUrl(urlString));
}



void Global::GlobalPrivate::setStatusBarText(QString const& text, bool permanent)
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
    global_private->setStatusBarText(text, permanent);
}

void Global::GlobalPrivate::addStatusBarPermanentItem(QWidget* widget)
{
    if(!m_statusBarExtension)
        return;

    m_statusBarExtension->addStatusBarItem(widget, 0, true);
}


void Global::addStatusBarPermanentItem(QWidget* widget)
{
    global_private->addStatusBarPermanentItem(widget);
}

void Global::GlobalPrivate::slotRemoveStatusBarLabel()
{
    m_statusBarExtension->removeStatusBarItem(m_statusBarLabel);
}


void Global::GlobalPrivate::slotStatusBarTimeout()
{
    if(!m_partDestroyed && m_statusBarExtension && m_statusBarExtension->statusBar())
        m_statusBarExtension->statusBar()->clearMessage();
}


#include "global.moc"
