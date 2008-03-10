/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                        *
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
#ifndef TABWIDGETSESSION_H
#define TABWIDGETSESSION_H

#include <ktabwidget.h>
#include <kurl.h>

class QToolButton;

#include "klinkstatus_export.h"
class SessionStackedWidget;
class SessionWidget;
class LinkStatus;


/**
This class handles the creation and destruction of sessions, i.e, severals instances of searching tabs.
 
@author Paulo Moura Guedes
*/
class KLINKSTATUS_EXPORT TabWidgetSession : public KTabWidget
{
    Q_OBJECT

public:
    explicit TabWidgetSession(QWidget * parent = 0, Qt::WFlags f = 0);
    ~TabWidgetSession();

    /** Set the URL in the current session widget */
    void setUrl(KUrl const& url);

    bool emptySessionsExist() const;
    /** Returns the first empty session it finds */
    SessionWidget* getEmptySession() const;
    
    SessionStackedWidget* currentWidget() const;
    SessionStackedWidget* widget(int index) const;

public slots:
    void slotNewSession(KUrl const& url = KUrl());
    void closeSession();
    void updateTabLabel(SessionStackedWidget*);
    void slotLoadSettings();

    void slotShowLinkCheckView();
    void slotShowUnreferredDocumentsView();
    void slotPreviousView();
    void slotNextView();
    void slotPreviousSession();
    void slotNextSession();
    
    void slotHideSearchPanel();
    void slotResetSearchOptions();
    void slotFollowLastLinkChecked();
    void slotDisableUpdatesOnResultsTable(bool checked);
    
    void slotStartSearch();
    void slotPauseSearch();
    void slotStopSearch();

    void slotRecheckVisibleItems();
    void slotRecheckBrokenItems();

    void slotExportAsHTML();
    void slotExportBrokenAsHTML();

    void slotFindUnreferredDocuments();

    // Tidy all the HTML documents present in the current session
    void slotValidateAll();
    
private slots:
    void slotCurrentChanged(int);
    
private:
    void addNewSession(KUrl const& url);

private:
    QToolButton* tabs_close_;
};

#endif
