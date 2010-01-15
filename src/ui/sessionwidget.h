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
 **************************************************************************/

#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "ui_sessionwidgetbase.h"
#include "engine/linkchecker.h"
#include "engine/linkstatus.h"
#include "ui/widgetinterface.h"
class SearchManager;
class TableItem;
class ActionManager;
class HttpPostDialog;

#include <QKeyEvent>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QDomDocument>
class QStringList;
class QTreeWidgetItem;

class KUrl;
class KConfig;
class KToggleAction;


class SessionTO
{
public:
    SessionTO()
      : depth(-1),
        checkParentFolders(true),
        checkExternalLinks(true),
        login(false)
    {}
  
    QDomDocument save(QDomDocument& doc) const;
    void load(QDomElement const& doc);

    QString url;
    int depth;
    bool checkParentFolders;
    bool checkExternalLinks;
    QString regExp;
    bool login;
    QString postUrl;
    QByteArray postData;
};


class SessionWidget: public PlayableWidgetInterface, public Ui_SessionWidgetBase
{
    Q_OBJECT

public:

    friend class SessionTO;

    explicit SessionWidget(int max_simultaneous_connections = 3, int time_out = 50,
                  QWidget* parent = 0);

    virtual ~SessionWidget();

    QString title() const;

    void setColumns(QStringList const& colunas);
    void setUrl(KUrl const& url);

    KUrl const& urlToCheck() const;
    
    bool treeDisplay() const { return tree_display_; }

    bool followLastLinkChecked() const { return follow_last_link_checked_; }
    void setFollowLastLinkChecked(bool follow);

    bool isEmpty() const;
    SearchManager* getSearchManager() const;

signals:
    void signalTitleChanged();
    void signalSearchStarted();
    void signalSearchPaused();
    void signalSearchFinnished();
    void signalUpdateActions();

public slots:

    void slotLoadSettings(bool modify_current_widget_settings = true);

    virtual bool supportsResuming();

    virtual void slotStartSearch();
    virtual void slotPauseSearch();
    virtual void slotStopSearch();

    void slotRecheckVisibleItems();
    void slotRecheckBrokenItems();

    void slotHideSearchPanel();
    void slotResetSearchOptions();
    void slotFollowLastLinkChecked();
    void slotDisableUpdatesOnResultsTable(bool checked);
    
    void slotExportAsHTML(LinkStatusHelper::Status status = LinkStatusHelper::none);
    void slotCreateSiteMap();
    
    void slotValidateAll();

private slots:

    virtual void slotCheck();
    virtual void slotCancel() {} // FIXME hack
    void slotLinkRecheck(LinkStatus*);

    void slotSearchStarted();
    void slotEnableCheckButton(const QString &);
    void slotRootChecked(LinkStatus* linkstatus);
    void slotLinkChecked(LinkStatus* linkstatus);
    void slotLinkRechecked(LinkStatus* linkstatus);
    void slotSearchFinished(SearchManager*);
    void slotSearchPaused();
    void slotSetTimeElapsed();
    void newSearchManager();

    void slotAddingLevel(bool adding);
    void slotLinksToCheckTotalSteps(int steps);
    void slotNewLinksToCheck(int numberOfLinks);
    // Happens when there is a redirection
    void slotIncrementLinksToCheckTotalSteps();

    void slotLoadSession(QString const& url);
    void slotChooseUrlDialog();

    void slotApplyFilter(LinkMatcher);
    
private:

    virtual void keyPressEvent ( QKeyEvent* e );
    bool validFields();
    void loadSession();
    void saveSession();
    void saveSessionSearchOptions();
    SessionTO buildTO() const;
    void loadSessionTO(SessionTO const& session);
    SessionTO buildSessionTO() const;
    void insertUrlAtCombobox(QString const& url);
    void init();
    void saveCurrentCheckSettings();
    bool pendingActions() const;
    void resetPendingActions();
    void loadResults();

private:
    KUrl url_to_check_;
  
    SearchManager* search_manager_;

    HttpPostDialog* login_dialog_;
    QTimer elapsed_time_timer_;
    QTime start_time_;
    int max_simultaneous_connections_;
    int time_out_;
    bool tree_display_; // tree/flat result display
    bool follow_last_link_checked_;
    bool check_in_background_;
    KToggleAction* start_search_action_;
};



#endif
