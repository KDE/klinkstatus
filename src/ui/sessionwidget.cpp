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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "sessionwidget.h"
#include "tablelinkstatus.h"
#include "treeview.h"
#include "klshistorycombo.h"
#include "resultview.h"
#include "../global.h"
#include "../engine/linkstatus.h"
#include "../engine/linkchecker.h"
#include "../engine/searchmanager.h"
#include "klsconfig.h"

#include <qevent.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qstringlist.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>

#include <kapplication.h>
#include <kurl.h>
#include <kcombobox.h>
#include <ksqueezedtextlabel.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kiconloader.h>


#include "plot2dcanvas.h"
#include <qvaluevector.h>
SessionWidget::SessionWidget(int max_simultaneous_connections, int time_out,
                             QWidget* parent, const char* name, WFlags f)
        : SessionWidgetBase(parent, name, f), search_manager_(0),
        ready_(true), bottom_status_timer_(this, "bottom_status_timer"),
        max_simultaneous_connections_(max_simultaneous_connections), time_out_(time_out)
{
    newSearchManager();

    combobox_url->init();
    slotLoadSettings();
    initIcons();

    connect(combobox_url, SIGNAL( textChanged ( const QString & ) ),
            this, SLOT( slotEnableCheckButton( const QString & ) ) );
    /*
    connect(table_linkstatus, SIGNAL( clicked ( int, int, int, const QPoint & ) ),
            this, SLOT( showBottomStatusLabel( int, int, int, const QPoint & ) ) );
    */
    connect(&bottom_status_timer_, SIGNAL(timeout()), this, SLOT(clearBottomStatusLabel()) );
}

SessionWidget::~SessionWidget()
{
    //combobox_url->saveItems(); This is done every time a URL is checked

    if(KLSConfig::rememberCheckSettings())
        saveCurrentCheckSettings();
}

void SessionWidget::slotLoadSettings(bool modify_current_widget_settings)
{
    if(modify_current_widget_settings)
    {
        checkbox_recursively->setChecked(KLSConfig::recursiveCheck());
        spinbox_depth->setValue(KLSConfig::depth());
        checkbox_subdirs_only->setChecked(!KLSConfig::checkParentFolders());
        checkbox_external_links->setChecked(KLSConfig::checkExternalLinks());
        tree_view->setRootIsDecorated(tree_display_);
        tree_display_ = KLSConfig::displayTreeView();
    }

    search_manager_->setTimeOut(KLSConfig::timeOut());
    
    //kdDebug(23100) << "tree_display_: " << tree_display_ << endl;
}

void SessionWidget::saveCurrentCheckSettings()
{
    KLSConfig::setRecursiveCheck(checkbox_recursively->isChecked());
    KLSConfig::setDepth(spinbox_depth->value());
    KLSConfig::setCheckParentFolders(!checkbox_subdirs_only->isChecked());
    KLSConfig::setCheckExternalLinks(checkbox_external_links->isChecked());

    KLSConfig::writeConfig();
}

void SessionWidget::newSearchManager()
{
    if(search_manager_)
        delete search_manager_;

    search_manager_ = new SearchManager(KLSConfig::maxConnectionsNumber(),
                                        KLSConfig::timeOut(),
                                        this, "search_manager");
    Q_ASSERT(search_manager_);

    connect(search_manager_, SIGNAL(signalRootChecked(const LinkStatus *, LinkChecker *)),
            this, SLOT(slotRootChecked(const LinkStatus *, LinkChecker *)));
    connect(search_manager_, SIGNAL(signalLinkChecked(const LinkStatus *, LinkChecker *)),
            this, SLOT(slotLinkChecked(const LinkStatus *, LinkChecker *)));
    connect(search_manager_, SIGNAL(signalSearchFinished()),
            this, SLOT(slotSearchFinished()));
    connect(search_manager_, SIGNAL(signalSearchPaused()),
            this, SLOT(slotSearchPaused()));
    connect(search_manager_, SIGNAL(signalAddingLevelTotalSteps(uint)),
            this, SLOT(slotAddingLevelTotalSteps(uint)));
    connect(search_manager_, SIGNAL(signalAddingLevelProgress()),
            this, SLOT(slotAddingLevelProgress()));
    connect(search_manager_, SIGNAL(signalLinksToCheckTotalSteps(uint)),
            this, SLOT(slotLinksToCheckTotalSteps(uint)));
}

void SessionWidget::setColumns(QStringList const& colunas)
{
    //table_linkstatus->setColumns(colunas);
    tree_view->setColumns(colunas);
}

void SessionWidget::setUrl(KURL const& url)
{
    combobox_url->setCurrentText(url.prettyURL());
    combobox_url->setFocus();
}

void SessionWidget::displayAllLinks()
{
    //table_linkstatus->showAll();
    tree_view->showAll();
}

void SessionWidget::displayGoodLinks()
{
    //table_linkstatus->show(ResultView::good);
    tree_view->show(ResultView::good);
}

void SessionWidget::displayBadLinks()
{
    //table_linkstatus->show(ResultView::bad);
    tree_view->show(ResultView::bad);
}

void SessionWidget::displayMalformedLinks()
{
    //table_linkstatus->show(ResultView::malformed);
    tree_view->show(ResultView::malformed);
}

void SessionWidget::displayUndeterminedLinks()
{
    //table_linkstatus->show(ResultView::undetermined);
    tree_view->show(ResultView::undetermined);
}

bool SessionWidget::isEmpty() const
{
    //Q_ASSERT(table_linkstatus);
    //return table_linkstatus->isEmpty();
    Q_ASSERT(tree_view);
    return tree_view->isEmpty();
}

SearchManager const* SessionWidget::getSearchManager() const
{
    return search_manager_;
}

void SessionWidget::slotEnableCheckButton(const QString & s)
{
    if(!s.isEmpty() && !search_manager_->searching())
        pushbutton_check->setEnabled(true);
    else
        pushbutton_check->setEnabled(false);
}
/*
void SessionWidget::slotSuggestDomain(bool toogle)
{
    if(toogle && !(combobox_url->currentText().isEmpty()))
    {
        KURL url = ::normalizeUrl(combobox_url->currentText());
        if(url.isValid())
            lineedit_domain->setText(url.host() + url.directory(true, false));
    }
}
*/
void SessionWidget::slotCheck()
{
    if(!ready_)
    {
        KApplication::beep ();
        return;
    }

    ready_ = false;
    if(!validFields())
    {
        ready_ = true;
        KApplication::beep ();
        return;
    }
    
    emit signalSearchStarted();
    slotLoadSettings(isEmpty()); // it seems that KConfigDialogManager is not trigering this slot

    newSearchManager();

    insertUrlAtCombobox(combobox_url->currentText());
    combobox_url->saveItems();
    progressbar_checker->reset();
    progressbar_checker->setPercentageVisible(true);
    progressbar_checker->setTotalSteps(1); // check root page
    progressbar_checker->setProgress(0);
    textlabel_progressbar->setText(i18n( "Checking..." ));

    //table_linkstatus->verticalHeader()->show();
    //table_linkstatus->verticalHeader()->adjustHeaderSize();
    //table_linkstatus->setLeftMargin(table_linkstatus->verticalHeader()->width());

    //buttongroup_search->setEnabled(false);
    pushbutton_check->setEnabled(false);
    pushbutton_cancel->setEnabled(true);
    pushbutton_cancel->setText(i18n( "&Pause" ));
    pushbutton_cancel->setIconSet(SmallIconSet("player_pause"));
    textlabel_elapsed_time->setEnabled(true);
    //textlabel_elapsed_time_value->setText("");
    textlabel_elapsed_time_value->setEnabled(true);

    Q_ASSERT(!pushbutton_check->isEnabled()); // FIXME pushbutton_check sometimes doesn't show disable. Qt bug?

    //table_linkstatus->clear();
    tree_view->clear();
    
    KURL url = ::normalizeUrl(combobox_url->currentText());
    if(KLSConfig::useQuantaUrlPreviewPrefix() && Global::isKLinkStatusEmbeddedInQuanta())
    {
        KURL url_aux = Global::urlWithQuantaPreviewPrefix(url);
        if(url_aux.isValid() && !url_aux.isEmpty())
            url = url_aux;
    }
    
    if(!checkbox_recursively->isChecked())
    {
        search_manager_->setSearchMode(SearchManager::depth);
        search_manager_->setDepth(0);
    }

    else if(checkbox_recursively->isChecked())
    {
        if(spinbox_depth->value() == 0)
        {
            search_manager_->setSearchMode(SearchManager::domain);
        }
        else
        {
            search_manager_->setSearchMode(SearchManager::depth_and_domain);
            search_manager_->setDepth(spinbox_depth->value());
        }

        if(checkbox_subdirs_only->isChecked())
        {
            search_manager_->setCheckParentDirs(false);

            if(url.hasHost())
                search_manager_->setDomain(url.host() + url.directory(true, false));
        }
        else
        {
            search_manager_->setCheckParentDirs(true);

            if(url.hasHost())
                search_manager_->setDomain(url.host());
        }
        if(checkbox_external_links->isChecked())
        {
            search_manager_->setCheckExternalLinks(true);
            search_manager_->setExternalDomainDepth(1);
        }
        else
        {
            search_manager_->setCheckExternalLinks(false);
            search_manager_->setExternalDomainDepth(0);
        }
    }

    kdDebug(23100) <<  "URI: " << url.prettyURL() << endl;
    combobox_url->setCurrentText(url.prettyURL());
    search_manager_->startSearch(url);
    slotSetTimeElapsed();
}

void SessionWidget::slotCancel()
{
    if(search_manager_->searching())
    {
        Q_ASSERT(!ready_);
        pushbutton_cancel->setEnabled(false);
        search_manager_->cancelSearch();
    }
    else
    {
        Q_ASSERT(ready_);
        Q_ASSERT(pushbutton_cancel->text() == i18n( "&Resume" ));
        pushbutton_check->setEnabled(false);
        pushbutton_cancel->setText(i18n( "&Pause" ));
        pushbutton_cancel->setIconSet(SmallIconSet("player_pause"));
        textlabel_progressbar->setText(i18n( "Checking..." ));
        ready_ = false;
        search_manager_->resume();
        
        displayAllLinks();
        emit signalSearchStarted();
        slotLoadSettings(isEmpty()); // it seems that KConfigDialogManager is not trigering this slot
    }
}

void SessionWidget::keyPressEvent ( QKeyEvent* e )
{
    if( e->key() == Qt::Key_Return &&
            ( combobox_url->hasFocus() ||
              //lineedit_domain->hasFocus() ||
              //checkbox_depth->hasFocus()  ||
              spinbox_depth->hasFocus()  ||
              //checkbox_domain->hasFocus()  ||
              //spinbox_external_domain->hasFocus()
              checkbox_recursively->hasFocus() ||
              checkbox_external_links->hasFocus() ||
              checkbox_subdirs_only->hasFocus() ) )
    {
        if(validFields())
        {
            pushbutton_check->toggle();
            //pushbutton_check->setEnabled(false);
            slotCheck();
        }
    }

    else if(e->key() == Qt::Key_F6)
    {
        combobox_url->lineEdit()->selectAll();
    }
}

bool SessionWidget::validFields()
{
    KURL url = ::normalizeUrl(combobox_url->currentText());

    if(combobox_url->currentText().isEmpty())
    {
        KMessageBox::sorry(this, i18n("Cowardly refusing to check an empty URL."));
        return false;
    }

    else
        return true;
}

void SessionWidget::slotRootChecked(LinkStatus const* linkstatus, LinkChecker * anal)
{
    slotSetTimeElapsed();
    emit signalUpdateTabLabel(search_manager_->linkStatusRoot());

    Q_ASSERT(textlabel_progressbar->text() == i18n( "Checking..." ));
    progressbar_checker->setProgress(1);

    //table_linkstatus->insertResult(linkstatus);
    TreeViewItem* tree_view_item = new TreeViewItem(tree_view, tree_view->lastItem(), linkstatus, 3);
    LinkStatus* ls = const_cast<LinkStatus*> (linkstatus);
    ls->setTreeViewItem(tree_view_item);

    if(linkstatus->isRedirection() && linkstatus->redirection())
        slotLinkChecked(linkstatus->redirection(), anal);
}

void SessionWidget::slotLinkChecked(LinkStatus const* linkstatus, LinkChecker * anal)
{
    slotSetTimeElapsed();

    Q_ASSERT(textlabel_progressbar->text() == i18n( "Checking..." ));
    progressbar_checker->setProgress(progressbar_checker->progress() + 1);

    if(linkstatus->checked())
    {
        TreeViewItem* tree_view_item = 0;
        
        if(tree_display_)
        {
            //kdDebug(23100) << "TREE!!!!!" << endl;
            TreeViewItem* parent_item = linkstatus->parent()->treeViewItem();
            tree_view_item = new TreeViewItem(parent_item, parent_item->lastChild(), linkstatus, 3);
            parent_item->setLastChild(tree_view_item);
            if(KLSConfig::followLastLinkChecked())
                tree_view->ensureRowVisible(tree_view_item, tree_display_);
            else
                tree_view->ensureRowVisible(tree_view->lastItem(), tree_display_);
        }
        else
        {
            //kdDebug(23100) << "FLAT!!!!!" << endl;
            tree_view_item = new TreeViewItem(tree_view, tree_view->lastItem(), linkstatus, 3);
            tree_view->ensureRowVisible(tree_view_item, tree_display_);
        }   
        LinkStatus* ls = const_cast<LinkStatus*> (linkstatus);
        ls->setTreeViewItem(tree_view_item);
        
        if(linkstatus->isRedirection() && linkstatus->redirection())
            slotLinkChecked(linkstatus->redirection(), anal);
    }
}
/*
vector<TableItem*> SessionWidget::generateRowOfTableItems(LinkStatus const* linkstatus) const
{
    vector<TableItem*> items;
    int column = 1;

    TableItem* item1 = new TableItemStatus(table_linkstatus, QTableItem::Never,
                                           linkstatus, column++);
    TableItem* item2 = new TableItemNome(table_linkstatus, QTableItem::Never,
                                         linkstatus, column++);
    TableItem* item3 = new TableItemURL(table_linkstatus, QTableItem::Never,
                                        linkstatus, column++);
    items.push_back(item1);
    items.push_back(item2);
    items.push_back(item3);

    // If more columns are choosed in the settings, create and add the items here
    // ...

    return items;
}
*/
void SessionWidget::slotSearchFinished()
{
    KApplication::beep ();

    textlabel_progressbar->setText(i18n( "Ready" ));
    progressbar_checker->reset();
    progressbar_checker->setPercentageVisible(false);
    progressbar_checker->setTotalSteps(1);
    progressbar_checker->setProgress(0);

    ready_ = true;
    pushbutton_check->setEnabled(true);
    pushbutton_cancel->setEnabled(false);
    //buttongroup_search->setEnabled(true);
    textlabel_elapsed_time->setEnabled(true);
    textlabel_elapsed_time_value->setEnabled(true);
    textlabel_elapsed_time_value->setText(search_manager_->timeElapsed().toString("hh:mm:ss"));

    emit signalSearchFinnished();
}

void SessionWidget::slotSearchPaused()
{
    KApplication::beep ();

    textlabel_progressbar->setText(i18n( "Stopped" ));

    ready_ = true;
    pushbutton_check->setEnabled(true);
    pushbutton_cancel->setEnabled(true);
    pushbutton_cancel->setText(i18n( "&Resume" ));
    pushbutton_cancel->setIconSet(SmallIconSet("player_play"));
    textlabel_elapsed_time->setEnabled(true);
    textlabel_elapsed_time_value->setEnabled(true);
    textlabel_elapsed_time_value->setText(search_manager_->timeElapsed().toString("hh:mm:ss"));

    emit signalSearchFinnished();
}

void SessionWidget::insertUrlAtCombobox(QString const& url)
{
    combobox_url->addToHistory(url);
}

void SessionWidget::showBottomStatusLabel(int row, int /*col*/, int /*button*/, QPoint const&  /*mousePos*/)
{
    if(table_linkstatus->myItem(row, 0))
    {
        QString status = table_linkstatus->myItem(row, 0)->toolTip(); // tooltip of the status column
        textlabel_status->setText(status);

        if(textlabel_status->sizeHint().width() > textlabel_status->maximumWidth())
            QToolTip::add
                (textlabel_status, status);
        else
            QToolTip::remove
                (textlabel_status);

        bottom_status_timer_.stop();
        bottom_status_timer_.start(5 * 1000, true);
    }
}

void SessionWidget::clearBottomStatusLabel()
{
    textlabel_status->clear();
}

void SessionWidget::slotSetTimeElapsed()
{
    textlabel_elapsed_time_value->setText(search_manager_->timeElapsed().toString("hh:mm:ss"));
}

void SessionWidget::slotAddingLevelTotalSteps(uint steps)
{
    textlabel_progressbar->setText(i18n( "Adding level..." ));
    progressbar_checker->reset();
    progressbar_checker->setTotalSteps(steps);
    progressbar_checker->setProgress(0);
}

void SessionWidget::slotAddingLevelProgress()
{
    Q_ASSERT(textlabel_progressbar->text() == i18n( "Adding level..." ));
    progressbar_checker->setProgress(progressbar_checker->progress() + 1);
}

void SessionWidget::slotLinksToCheckTotalSteps(uint steps)
{
    textlabel_progressbar->setText(i18n( "Checking..." ));
    progressbar_checker->reset();
    progressbar_checker->setTotalSteps(steps);
    progressbar_checker->setProgress(0);
}

void SessionWidget::initIcons()
{
    pushbutton_check->setIconSet(SmallIconSet("viewmag"));
    pushbutton_cancel->setIconSet(SmallIconSet("player_pause"));
    toolButton_clear_combo->setIconSet(SmallIconSet("locationbar_erase"));
}

void SessionWidget::slotClearComboUrl()
{
    combobox_url->setCurrentText("");
}


#include "sessionwidget.moc"
