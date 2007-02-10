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

#include <klocale.h>
#include <kapplication.h>
#include <kurl.h>
#include <kcombobox.h>
#include <ksqueezedtextlabel.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kicon.h>
#include <kglobal.h>
#include <kpushbutton.h>
#include <kfiledialog.h>

#include <ktemporaryfile.h>
#include <ksavefile.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>

#include <QEvent>
#include <QLineEdit>
#include <qspinbox.h>
#include <QCheckBox>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <q3listbox.h>
#include <QStringList>
#include <q3buttongroup.h>
#include <QToolButton>
#include <QRegExp>
#include <ktoggleaction.h>
//Added by qt3to4:
#include <QPixmap>
#include <QKeyEvent>
//Added by qt3to4:
#include <QTextStream>

#include "sessionwidget.h"
#include "treeview.h"
#include "documentrootdialog.h"
#include "klshistorycombo.h"
#include "klsconfig.h"
#include "resultview.h"
#include "../global.h"
#include "../engine/linkstatus.h"
#include "../engine/linkchecker.h"
#include "../engine/searchmanager.h"
#include "resultssearchbar.h"
#include "../actionmanager.h"
#include "../utils/utils.h"
#include "../utils/xsl.h"


SessionWidget::SessionWidget(int max_simultaneous_connections, int time_out,
                             QWidget* parent, const char* name, Qt::WFlags f)
    : QWidget(parent, name, f), search_manager_(0),
        action_manager_(ActionManager::getInstance()),
        ready_(true), to_start_(false), to_pause_(false), to_stop_(false),
        in_progress_(false), paused_(false), stopped_(true),
        bottom_status_timer_(this),
        max_simultaneous_connections_(max_simultaneous_connections),
        time_out_(time_out), tree_display_(false), follow_last_link_checked_(KLSConfig::followLastLinkChecked()),
        start_search_action_(0)
{
    setupUi(this);
    
    newSearchManager();

    init();
    slotLoadSettings();

    connect(combobox_url, SIGNAL( textChanged ( const QString & ) ),
            this, SLOT( slotEnableCheckButton( const QString & ) ) );

    connect(tree_view, SIGNAL( clicked ( Q3ListViewItem * ) ),
            this, SLOT( showBottomStatusLabel( Q3ListViewItem * ) ) );

    connect(&bottom_status_timer_, SIGNAL(timeout()), this, SLOT(clearBottomStatusLabel()) );
}

SessionWidget::~SessionWidget()
{
    //combobox_url->saveItems(); This is done every time a URL is checked

    if(KLSConfig::rememberCheckSettings())
        saveCurrentCheckSettings();
}

void SessionWidget::init()
{
    combobox_url->init();

    toolButton_clear_combo->setIcon(KIcon("locationbar_erase"));

    pushbutton_url->setIcon(KIcon("fileopen"));
    const int pixmapSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    pushbutton_url->setFixedSize( pixmapSize+8, pixmapSize+8 );
    connect(pushbutton_url, SIGNAL(clicked()), this, SLOT(slotChooseUrlDialog()));

    resultsSearchBar->hide();

    start_search_action_ = static_cast<KToggleAction*> (action_manager_->action("start_search"));

    connect(resultsSearchBar, SIGNAL(signalSearch(LinkMatcher)),
            this, SLOT(slotApplyFilter(LinkMatcher)));
}

void SessionWidget::slotLoadSettings(bool modify_current_widget_settings)
{
    if(modify_current_widget_settings)
    {
        checkbox_recursively->setChecked(KLSConfig::recursiveCheck());
        spinbox_depth->setValue(KLSConfig::depth());
        checkbox_subdirs_only->setChecked(!KLSConfig::checkParentFolders());
        checkbox_external_links->setChecked(KLSConfig::checkExternalLinks());
        tree_display_ = KLSConfig::displayTreeView();
        tree_view->setTreeDisplay(tree_display_);
    }

    search_manager_->setTimeOut(KLSConfig::timeOut());

    //kDebug(23100) << "tree_display_: " << tree_display_ << endl;
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
                                        this);
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
    tree_view->setColumns(colunas);
}

void SessionWidget::setUrl(KUrl const& url)
{
  combobox_url->setItemText(combobox_url->currentIndex(), url.prettyUrl());
    combobox_url->setFocus();
}

bool SessionWidget::isEmpty() const
{
    Q_ASSERT(tree_view);
    return tree_view->isEmpty();
}

SearchManager const* SessionWidget::getSearchManager() const
{
    return search_manager_;
}

void SessionWidget::slotEnableCheckButton(const QString & s)
{
    if(!(stopped_ && !pendingActions()))
        return;

    if(!s.isEmpty() && !search_manager_->searching())
    {
        start_search_action_->setEnabled(true);
    }
    else
    {
        start_search_action_->setEnabled(false);
    }
}

void SessionWidget::slotCheck()
{
    Q_ASSERT(to_start_);
    Q_ASSERT(!in_progress_);
    Q_ASSERT(!paused_);
    Q_ASSERT(stopped_);

    ready_ = false;
    if(!validFields())
    {
        ready_ = true;
        KApplication::beep();
        return;
    }

    emit signalSearchStarted();

    in_progress_ = true;
    paused_ = false;
    stopped_ = false;

    slotLoadSettings(false); // it seems that KConfigDialogManager is not trigering this slot

    newSearchManager();

    // WORKAROUND addToHistory breaks currentText()
    QString current_text = combobox_url->currentText();
    insertUrlAtCombobox(current_text);

    combobox_url->saveItems();
    progressbar_checker->reset();
    progressbar_checker->setTextVisible(true);
    progressbar_checker->setRange(0, 1); // check root page
    progressbar_checker->setValue(0);
    textlabel_progressbar->setText(i18n( "Checking..." ));

    textlabel_elapsed_time->setEnabled(true);
    //textlabel_elapsed_time_value->setText("");
    textlabel_elapsed_time_value->setEnabled(true);

    //table_linkstatus->clear();
    tree_view->clear();

    KUrl url = Url::normalizeUrl(current_text);

    if(!url.protocol().startsWith("http"))
    {
        KUrl dir = url;
        dir.setPath(url.directory());
        DocumentRootDialog dialog(this, dir);
        dialog.exec();
        search_manager_->setDocumentRoot(dialog.url());
    }

    if(KLSConfig::useQuantaUrlPreviewPrefix() && Global::isKLinkStatusEmbeddedInQuanta())
    {
        KUrl url_aux = Global::urlWithQuantaPreviewPrefix(url);
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
                search_manager_->setDomain(url.host()
                    + url.directory(KUrl::AppendTrailingSlash | KUrl::ObeyTrailingSlash));
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
    if(!lineedit_reg_exp->text().isEmpty())
    {
        search_manager_->setCheckRegularExpressions(true);
        search_manager_->setRegularExpression(lineedit_reg_exp->text(), false);
    }

    kDebug(23100) <<  "URI: " << url.prettyUrl() << endl;
    combobox_url->setItemText(combobox_url->currentIndex(), url.prettyUrl());
    search_manager_->startSearch(url);
    slotSetTimeElapsed();
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
        slotStartSearch();
    }

    else if(e->key() == Qt::Key_F6)
    {
        combobox_url->lineEdit()->selectAll();
    }
}

bool SessionWidget::validFields()
{
    QString url_string = combobox_url->currentText();
//     kDebug() << "SessionWidget::validFields: " << url_string << endl;
    if(url_string.isEmpty())
    {
        KMessageBox::sorry(this, i18n("Cowardly refusing to check an empty URL."));
        return false;
    }
    return true;
}

void SessionWidget::slotRootChecked(LinkStatus const* linkstatus, LinkChecker * anal)
{
    slotSetTimeElapsed();
    emit signalUpdateTabLabel(search_manager_->linkStatusRoot(), this);

    Q_ASSERT(textlabel_progressbar->text() == i18n("Checking...") ||
            textlabel_progressbar->text() == i18n("Stopped"));
    progressbar_checker->setValue(1);

    //table_linkstatus->insertResult(linkstatus);
    TreeViewItem* tree_view_item = new TreeViewItem(tree_view, tree_view->lastItem(), linkstatus);
    LinkStatus* ls = const_cast<LinkStatus*> (linkstatus);
    ls->setTreeViewItem(tree_view_item);

    if(linkstatus->isRedirection() && linkstatus->redirection())
        slotLinkChecked(linkstatus->redirection(), anal);

    resultsSearchBar->show();
    ActionManager::getInstance()->action("file_export_html")->setEnabled(!isEmpty());
}

void SessionWidget::slotLinkChecked(LinkStatus const* linkstatus, LinkChecker * anal)
{
    slotSetTimeElapsed();

    kDebug(23100) << textlabel_progressbar->text() << endl;
    Q_ASSERT(textlabel_progressbar->text() == i18n("Checking...") ||
            textlabel_progressbar->text() == i18n("Stopped"));
    progressbar_checker->setValue(progressbar_checker->value() + 1);

    if(linkstatus->checked())
    {
        TreeViewItem* tree_view_item = 0;
        TreeViewItem* parent_item = linkstatus->parent()->treeViewItem();
        bool match = resultsSearchBar->currentLinkMatcher().matches(*linkstatus);

        if(tree_display_)
        {
            //kDebug(23100) << "TREE!!!!!" << endl;
            tree_view_item = new TreeViewItem(tree_view, parent_item, parent_item->lastChild(), linkstatus);
            parent_item->setLastChild(tree_view_item);
            if(follow_last_link_checked_)
                tree_view->ensureRowVisible(tree_view_item, tree_display_);

            tree_view_item->setEnabled(match);
        }
        else
        {
            //kDebug(23100) << "FLAT!!!!!" << endl;
            tree_view_item = new TreeViewItem(tree_view, tree_view->lastItem(), linkstatus);
            if(follow_last_link_checked_)
                tree_view->ensureRowVisible(tree_view_item, tree_display_);

            tree_view_item->setVisible(match);
        }

        LinkStatus* ls = const_cast<LinkStatus*> (linkstatus);
        ls->setTreeViewItem(tree_view_item);

        if(linkstatus->isRedirection() && linkstatus->redirection())
            slotLinkChecked(linkstatus->redirection(), anal);
    }
}

void SessionWidget::slotSearchFinished()
{
    Q_ASSERT(in_progress_);
    Q_ASSERT(!paused_);
    Q_ASSERT(!stopped_);

    KApplication::beep ();

    textlabel_progressbar->setText(i18n( "Ready" ));
    progressbar_checker->reset();
    progressbar_checker->setTextVisible(false);
    progressbar_checker->setRange(0, 1);
    progressbar_checker->setValue(0);

    ready_ = true;

    textlabel_elapsed_time->setEnabled(true);
    textlabel_elapsed_time_value->setEnabled(true);
    textlabel_elapsed_time_value->setText(search_manager_->timeElapsed().toString("hh:mm:ss"));

    in_progress_ = false;
    paused_ = false;
    stopped_ = true;
    resetPendingActions();
    action_manager_->slotUpdateSessionWidgetActions(this);

    emit signalSearchFinnished();
}

void SessionWidget::slotSearchPaused()
{
    Q_ASSERT(pendingActions());
    Q_ASSERT(in_progress_);

    KApplication::beep();

    textlabel_progressbar->setText(i18n("Stopped"));

    ready_ = true;

    if(to_stop_)
    {
        in_progress_ = false;
        paused_ = false;
        stopped_ = true;
    }
    else
    {
        Q_ASSERT(to_pause_);
        Q_ASSERT(!stopped_);

        paused_ = true;
    }

    textlabel_elapsed_time->setEnabled(true);
    textlabel_elapsed_time_value->setEnabled(true);
    textlabel_elapsed_time_value->setText(search_manager_->timeElapsed().toString("hh:mm:ss"));

    resetPendingActions();
    action_manager_->slotUpdateSessionWidgetActions(this);

    emit signalSearchPaused();
}

void SessionWidget::insertUrlAtCombobox(QString const& url)
{
//     kDebug() << "SessionWidget::insertUrlAtCombobox: " << url << endl;
    combobox_url->addToHistory(url);
}

void SessionWidget::showBottomStatusLabel(Q3ListViewItem* item)
{
    kDebug(23100) << "SessionWidget::showBottomStatusLabel" << endl;

    if(!item)
        return;
    
    TreeViewItem* _item = tree_view->myItem(item);
    if(_item)
    {
        QString status = _item->linkStatus()->statusText();
        textlabel_status->setText(status);

        if(textlabel_status->sizeHint().width() > textlabel_status->maximumWidth())
            textlabel_status->setToolTip(status);
        else
            textlabel_status->setToolTip(QString());

        bottom_status_timer_.stop();
        bottom_status_timer_.start(5 * 1000);
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
    progressbar_checker->setRange(0, steps);
    progressbar_checker->setValue(0);
}

void SessionWidget::slotAddingLevelProgress()
{
    Q_ASSERT(textlabel_progressbar->text() == i18n( "Adding level..." ));
    progressbar_checker->setValue(progressbar_checker->value() + 1);
}

void SessionWidget::slotLinksToCheckTotalSteps(uint steps)
{
    textlabel_progressbar->setText(i18n( "Checking..." ));
    progressbar_checker->reset();
    progressbar_checker->setRange(0, steps);
    progressbar_checker->setValue(0);
}

void SessionWidget::slotClearComboUrl()
{
    combobox_url->setItemText(combobox_url->currentIndex(), "");
}

void SessionWidget::slotChooseUrlDialog()
{
    setUrl(KFileDialog::getOpenUrl());
}

void SessionWidget::slotHideSearchPanel()
{
    if(searchGroupBox->isHidden())
        searchGroupBox->show();
    else
        searchGroupBox->hide();
}

void SessionWidget::setFollowLastLinkChecked(bool follow)
{
    kDebug(23100) << "setFollowLastLinkChecked: " << follow << endl;
    follow_last_link_checked_ = follow;
}

void SessionWidget::slotFollowLastLinkChecked()
{
    follow_last_link_checked_ = !follow_last_link_checked_;
}

void SessionWidget::slotResetSearchOptions()
{
    slotLoadSettings(true);

    combobox_url->clear();
    lineedit_reg_exp->clear();
}

void SessionWidget::slotStartSearch()
{
    if(in_progress_)
    {
        start_search_action_->setChecked(true); // do not toggle
        Q_ASSERT(!stopped_);
        KApplication::beep();
        return;
    }

    to_start_ = true;
    slotLoadSettings(false);
    slotCheck();
    resetPendingActions();

    action_manager_->slotUpdateSessionWidgetActions(this);
}

void SessionWidget::slotPauseSearch()
{
    Q_ASSERT(in_progress_);
    Q_ASSERT(!stopped_);

    if(pendingActions())
        return;

    to_pause_ = true;

    if(!paused_)
    {
        Q_ASSERT(!ready_);
        Q_ASSERT(search_manager_->searching());

        search_manager_->cancelSearch();
    }
    else
    {
        Q_ASSERT(ready_);

        paused_ = false;

        textlabel_progressbar->setText(i18n("Checking..."));
        ready_ = false;
        search_manager_->resume();

        emit signalSearchStarted();
        slotLoadSettings(isEmpty()); // it seems that KConfigDialogManager is not trigering this slot

        resetPendingActions();
    }
}

void SessionWidget::slotStopSearch()
{
    Q_ASSERT(in_progress_);
    Q_ASSERT(!stopped_);

    if(pendingActions())
        return;

    to_stop_ = true;

    if(!paused_)
    {
        Q_ASSERT(!ready_);
        Q_ASSERT(search_manager_->searching());

        search_manager_->cancelSearch();
    }
    else
    {
        in_progress_ = false;
        paused_ = false;
        stopped_ = true;

        action_manager_->slotUpdateSessionWidgetActions(this);
    }
}

bool SessionWidget::pendingActions() const
{
    return (to_start_ || to_pause_ || to_stop_);
}

void SessionWidget::resetPendingActions()
{
    to_start_ = false;
    to_pause_ = false;
    to_stop_ = false;
}

void SessionWidget::slotApplyFilter(LinkMatcher link_matcher)
{
    tree_view->show(link_matcher);
}

void SessionWidget::slotExportAsHTML( )
{
    KUrl url = KFileDialog::getSaveUrl(KUrl(),"text/html", 0, i18n("Export Results as HTML"));

    if(url.isEmpty())
        return;

    QString filename;

    if(url.isLocalFile())
        filename = url.path();
    else {
        KTemporaryFile tmp; // ### only used for network export
        tmp.setAutoRemove(false);
        tmp.open();
        filename = tmp.fileName();
    }

    KSaveFile *savefile = new KSaveFile(filename);
    if(savefile->open()) // ok
    {
        QTextStream outputStream ( savefile );
        outputStream.setCodec(QTextCodec::codecForName("UTF-8"));

        QString xslt_doc = FileManager::read(KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl"));
        XSLT xslt(xslt_doc);
//         kDebug(23100) << search_manager_->toXML() << endl;
        QString html_ouptut = xslt.transform(search_manager_->toXML());
        outputStream << html_ouptut << endl;
        outputStream.flush();
    }

    delete savefile;

    if (url.isLocalFile())
        return;

    KIO::NetAccess::upload(filename, url, 0);
}


#include "sessionwidget.moc"
