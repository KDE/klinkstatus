/***************************************************************************
 *   Copyright (C) 2007 by Paulo Moura Guedes                              *
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

#ifndef SESSION_STACKED_WIDGET_H
#define SESSION_STACKED_WIDGET_H

#include <QStackedWidget>

class SessionWidget;
class UnreferredDocumentsWidget;

class KUrl;

    
class SessionStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    SessionStackedWidget(QWidget* parent = 0);
    ~SessionStackedWidget();

    SessionWidget* sessionWidget() const;
    UnreferredDocumentsWidget* unreferredDocumentsWidget() const;

    bool isSessionWidgetActive() const;
    bool isUnreferredDocumentsWidgetActive() const;

    void addUnreferredDocumentsWidget(UnreferredDocumentsWidget* widget, bool setCurrentWidget = false);

Q_SIGNALS:
    void signalTitleChanged(SessionStackedWidget*);
  
private Q_SLOTS:
    void slotChangeTitle();
    void slotUpdateActions();
    
private:
    SessionWidget* m_sessionWidget;
    UnreferredDocumentsWidget* m_unreferredDocumentsWidget;
};

#endif
