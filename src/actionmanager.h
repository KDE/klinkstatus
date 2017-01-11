/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   kde@mouraguedes.com                                                        *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/
#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>

class QAction;
class KActionCollection;

#include "klinkstatus_export.h"
class SessionWidget;
class SessionStackedWidget;
class KLinkStatusPart;
class TabWidgetSession;

/**
    @author Paulo Moura Guedes <kde@mouraguedes.com>

    interface for accessing actions, popup menus etc. from widgets.
*/
class KLINKSTATUS_EXPORT ActionManager : public QObject
{
    Q_OBJECT
public:
    ActionManager(QObject* parent = 0);
    virtual ~ActionManager();

    static ActionManager* getInstance();
    static void setInstance(ActionManager* manager);

    virtual QAction* action(const QString & name);
    virtual QWidget* container(const char* name);

    KActionCollection* actionCollection();

    void initPart(KLinkStatusPart* part);
    void initSessionWidget(SessionWidget* sessionWidget);
    void initTabWidget(TabWidgetSession* tabWidgetSession);

public slots:
    void slotUpdateActions(SessionStackedWidget*);

private slots:
    void slotFillGotoViewPopup();
    void slotGHNS();
    
private:
    // start, pause and stop
    void updatePlayActions(SessionStackedWidget*);
    void updateGeneralActions(SessionStackedWidget*);

private:

    static ActionManager* m_self;

    class ActionManagerPrivate;
    ActionManagerPrivate* d;
};

#endif
