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

#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "sessionwidgetbase.h"
#include "../engine/linkchecker.h"
#include "../engine/linkstatus.h"
class SearchManager;
class TableItem;
class ActionManager;
class LinkMatcher;

#include <qtimer.h>
#include <qstring.h>
class QStringList;
class QListViewItem;

class KURL;
class KConfig;
class KToggleAction;

#include <vector>

using namespace std;

class SessionWidget: public SessionWidgetBase
{
    Q_OBJECT

public:

    SessionWidget(int max_simultaneous_connections = 3, int time_out = 50,
                  QWidget* parent = 0, const char* name = 0, WFlags f = 0);

    ~SessionWidget();

    void setColumns(QStringList const& colunas);
    void setUrl(KURL const& url);
    
    bool treeDisplay() const { return tree_display_; }

    bool followLastLinkChecked() const { return follow_last_link_checked_; }
    void setFollowLastLinkChecked(bool follow);

    bool isEmpty() const;
    SearchManager const* getSearchManager() const;

    bool inProgress() const { return in_progress_; }
    bool paused() const { return paused_; }
    bool stopped() const { return stopped_; }

signals:
    void signalUpdateTabLabel(const LinkStatus *, SessionWidget*);
    void signalSearchStarted();
    void signalSearchPaused();
    void signalSearchFinnished();

public slots:

    virtual void slotClearComboUrl();
    void slotLoadSettings(bool modify_current_widget_settings = true);

    void slotStartSearch();
    void slotPauseSearch();
    void slotStopSearch();

    void slotHideSearchPanel();
    void slotResetSearchOptions();
    void slotFollowLastLinkChecked();
    
    void slotExportAsHTML();

private slots:

    virtual void slotCheck();
    virtual void slotCancel() {} // FIXME hack
    //virtual void slotSuggestDomain(bool toogle);

    void slotEnableCheckButton(const QString &);
    void slotRootChecked(LinkStatus const* linkstatus, LinkChecker * anal);
    void slotLinkChecked(LinkStatus const* linkstatus, LinkChecker * anal);
    void slotSearchFinished();
    void slotSearchPaused();
    /** Shows the status of the clicked URL (row) for 5 seconds */
    void showBottomStatusLabel(QListViewItem* item);
    void clearBottomStatusLabel();
    void slotSetTimeElapsed();
    void newSearchManager();

    void slotAddingLevelTotalSteps(uint steps);
    void slotAddingLevelProgress();
    void slotLinksToCheckTotalSteps(uint steps);

    void slotChooseUrlDialog();

    void slotApplyFilter(LinkMatcher);
    
private:

    virtual void keyPressEvent ( QKeyEvent* e );
    bool validFields();
    void insertUrlAtCombobox(QString const& url);
    void init();
    void saveCurrentCheckSettings();
    bool pendingActions() const;
    void resetPendingActions();

private:
    SearchManager* search_manager_;
    ActionManager* action_manager_;

    bool ready_;
    bool to_start_;
    bool to_pause_;
    bool to_stop_;
    bool in_progress_;
    bool paused_;
    bool stopped_;

    QTimer bottom_status_timer_;
    int max_simultaneous_connections_;
    int time_out_;
    bool tree_display_; // tree/flat result display
    bool follow_last_link_checked_;
    KToggleAction* start_search_action_;
};



#endif
