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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SESSION_WIDGET_H
#define SESSION_WIDGET_H

#include "sessionwidgetbase.h"
#include "../engine/linkchecker.h"
#include "../engine/linkstatus.h"
class SearchManager;
class TableItem;
class TableLinkstatus; // TODO remove this when possible

#include <qtimer.h>
#include <qstring.h>
class QStringList;

#include <vector>

using namespace std;

class KURL;
class KConfig;

class SessionWidget: public SessionWidgetBase
{
    Q_OBJECT

public:

    SessionWidget(int max_simultaneous_connections = 3, int time_out = 50,
                  QWidget* parent = 0, const char* name = 0, WFlags f = 0);

    ~SessionWidget();

    void setColumns(QStringList const& colunas);
    void setUrl(KURL const& url);
    void displayAllLinks();
    void displayGoodLinks();
    void displayBadLinks();
    void displayMalformedLinks();
    void displayUndeterminedLinks();



    bool isEmpty() const;
    SearchManager const* getSearchManager() const;

signals:
    void signalUpdateTabLabel(const LinkStatus *);
    void signalSearchStarted();
    void signalSearchFinnished();
    
public slots:
    
    virtual void slotClearComboUrl();    
    void slotLoadSettings(bool modify_current_widget_settings = true);
    
private slots:

    virtual void slotCheck();
    virtual void slotCancel();
    //virtual void slotSuggestDomain(bool toogle);

    void slotEnableCheckButton(const QString &);
    void slotRootChecked(LinkStatus const* linkstatus, LinkChecker * anal);
    void slotLinkChecked(LinkStatus const* linkstatus, LinkChecker * anal);
    void slotSearchFinished();
    void slotSearchPaused();
    /** Shows the status of the clicked URL (row) for 5 seconds */
    void showBottomStatusLabel(int row, int col, int button, QPoint const&  mousePos);
    void clearBottomStatusLabel();
    void slotSetTimeElapsed();
    void newSearchManager();

    void slotAddingLevelTotalSteps(uint steps);
    void slotAddingLevelProgress();
    void slotLinksToCheckTotalSteps(uint steps);

private:

    virtual void keyPressEvent ( QKeyEvent* e );
    bool validFields();
    //vector<TableItem*> generateRowOfTableItems(LinkStatus const* linkstatus) const;
    void insertUrlAtCombobox(QString const& url);
    void initIcons();
    void saveCurrentCheckSettings();

private:
    SearchManager* search_manager_;
    bool ready_;
    QTimer bottom_status_timer_;
    int max_simultaneous_connections_;
    int time_out_;
    TableLinkstatus* table_linkstatus;
    bool tree_display_; // tree/flat result display
};



#endif
