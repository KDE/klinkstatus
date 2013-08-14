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

#include "sessionwidget.h"

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
#include <ktoggleaction.h>
#include <kstringhandler.h>
#include <kcharsets.h>
#include <kmimetype.h>
#include <kio/netaccess.h>

#include <QEvent>
#include <QLineEdit>
#include <qspinbox.h>
#include <QCheckBox>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QStringList>
#include <QToolButton>
#include <QRegExp>
#include <QPixmap>
#include <QKeyEvent>
#include <QTextStream>
#include <QTreeWidget>
#include <QProcess>

#include "ui/treeview.h"
#include "ui/documentrootdialog.h"
#include "ui/klshistorycombo.h"
#include "ui/resultssearchbar.h"
#include "ui/resultview.h"
#include "ui/httppostdialog.h"
#include "klsconfig.h"
#include "global.h"
#include "engine/linkstatus.h"
#include "engine/searchmanager.h"
#include "actionmanager.h"
#include "utils/utils.h"


SessionWidget::SessionWidget(int max_simultaneous_connections, int time_out,
                             QWidget* parent)
    : PlayableWidgetInterface(parent), search_manager_(0),
        login_dialog_(0), elapsed_time_timer_(this), 
        max_simultaneous_connections_(max_simultaneous_connections),
        time_out_(time_out), tree_display_(false), follow_last_link_checked_(KLSConfig::followLastLinkChecked()),
        check_in_background_(false), start_search_action_(0)
{
    setupUi(this);
    
    init();
    slotLoadSettings();

    connect(this, SIGNAL(signalSearchStarted()),
            this, SLOT(slotSearchStarted()));

    connect(combobox_url, SIGNAL(editTextChanged(const QString&)),
            this, SLOT(slotEnableCheckButton(const QString&)));

    connect(&elapsed_time_timer_, SIGNAL(timeout()),
             this, SLOT(slotSetTimeElapsed()));
}

SessionWidget::~SessionWidget()
{
    if(KLSConfig::rememberCheckSettings())
        saveCurrentCheckSettings();
}

void SessionWidget::init()
{
    combobox_url->init();

    pushbutton_url->setIcon(KIcon("document-open"));
    const int pixmapSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    pushbutton_url->setFixedSize(pixmapSize + 8, pixmapSize + 8);
    
    connect(pushbutton_url, SIGNAL(clicked()), this, SLOT(slotChooseUrlDialog()));
    connect(combobox_url, SIGNAL(editTextChanged(const QString&)),
            this, SLOT(slotLoadSession(const QString&)));
    connect(combobox_url, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(slotLoadSession(const QString&)));
    
    textedit_elapsed_time_value->setText(QTime(0, 0).toString("hh:mm:ss"));

    resultsSearchBar->hide();

    start_search_action_ = static_cast<KToggleAction*> (ActionManager::getInstance()->action("start_search"));

    connect(tree_view, SIGNAL(signalLinkRecheck(LinkStatus*)),
            this, SLOT(slotLinkRecheck(LinkStatus*)));

    connect(resultsSearchBar, SIGNAL(signalSearch(LinkMatcher)),
            this, SLOT(slotApplyFilter(LinkMatcher)));

    elapsed_time_timer_.setInterval(1000);
}

void SessionWidget::slotLoadSettings(bool modify_current_widget_settings)
{
    if(modify_current_widget_settings)
    {
        spinbox_depth->setValue(KLSConfig::depth());
        checkbox_subdirs_only->setChecked(!KLSConfig::checkParentFolders());
        checkbox_external_links->setChecked(KLSConfig::checkExternalLinks());
    }

    tree_display_ = KLSConfig::displayTreeView();
    tree_view->setTreeDisplay(tree_display_);

    if(search_manager_)
        search_manager_->setTimeOut(KLSConfig::timeOut());

    KToggleAction* action = static_cast<KToggleAction*> (ActionManager::getInstance()->action("search_in_background"));
    if(action) {
        check_in_background_ = action->isChecked();
    }
        
    //kDebug(23100) << "tree_display_: " << tree_display_;
}

void SessionWidget::saveCurrentCheckSettings()
{
    KLSConfig::setDepth(spinbox_depth->value());
    KLSConfig::setCheckParentFolders(!checkbox_subdirs_only->isChecked());
    KLSConfig::setCheckExternalLinks(checkbox_external_links->isChecked());
    KToggleAction* followLinks = static_cast<KToggleAction*> (ActionManager::getInstance()->action("follow_last_link_checked"));
    KLSConfig::setFollowLastLinkChecked(followLinks->isChecked());

    KLSConfig::self()->writeConfig();
}

void SessionWidget::newSearchManager()
{
    if(search_manager_)
        delete search_manager_;

    search_manager_ = new SearchManager(KLSConfig::maxConnectionsNumber(),
                                        KLSConfig::timeOut(),
                                        this);

    connect(search_manager_, SIGNAL(signalRootChecked(LinkStatus*)),
            this, SLOT(slotRootChecked(LinkStatus*)));
    connect(search_manager_, SIGNAL(signalLinkChecked(LinkStatus*)),
            this, SLOT(slotLinkChecked(LinkStatus*)));
    connect(search_manager_, SIGNAL(signalSearchFinished(SearchManager*)),
            this, SLOT(slotSearchFinished(SearchManager*)));
    connect(search_manager_, SIGNAL(signalSearchPaused()),
            this, SLOT(slotSearchPaused()));
    connect(search_manager_, SIGNAL(signalAddingLevel(bool)),
            this, SLOT(slotAddingLevel(bool)));
    connect(search_manager_, SIGNAL(signalNewLinksToCheck(int)),
            this, SLOT(slotNewLinksToCheck(int)));
    connect(search_manager_, SIGNAL(signalLinksToCheckTotalSteps(int)),
            this, SLOT(slotLinksToCheckTotalSteps(int)));
    connect(search_manager_, SIGNAL(signalLinkRechecked(LinkStatus*)),
            this, SLOT(slotLinkRechecked(LinkStatus*)));
    connect(search_manager_, SIGNAL(signalRedirection()),
            this, SLOT(slotIncrementLinksToCheckTotalSteps()));
}

void SessionWidget::setColumns(QStringList const& colunas)
{
    tree_view->setColumns(colunas);
}

void SessionWidget::setUrl(KUrl const& url)
{
    combobox_url->addCurrentItem(url.prettyUrl());
    combobox_url->setFocus();
}

bool SessionWidget::isEmpty() const
{
    Q_ASSERT(tree_view);
    return tree_view->isEmpty();
}

SearchManager* SessionWidget::getSearchManager() const
{
    return search_manager_;
}

void SessionWidget::slotEnableCheckButton(const QString & s)
{
    if(!(stopped_ && !pendingActions()))
        return;

    if(!s.isEmpty() && (!search_manager_ || !search_manager_->searching()))
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

    in_progress_ = true;
    paused_ = false;
    stopped_ = false;

    newSearchManager();
    slotLoadSettings(false); // it seems that KConfigDialogManager is not trigering this slot

    // WORKAROUND addToHistory breaks currentText()
    QString current_text = combobox_url->currentText();

//     saveSession();

    progressbar_checker->reset();
    progressbar_checker->setTextVisible(true);
    progressbar_checker->setRange(0, 1); // check root page
    progressbar_checker->setValue(0);
    textlabel_progressbar->setText(i18n( "Checking..." ));

    textlabel_elapsed_time->setEnabled(true);
    textedit_elapsed_time_value->setEnabled(true);
    label_checked_links->setEnabled(true);
    textedit_checked_links->setEnabled(true);

    //table_linkstatus->clear();
    tree_view->clear();

    KUrl url = Url::normalizeUrl(current_text);

    if(!url.protocol().startsWith("http"))
    {
        checkBoxLogin->setCheckState(Qt::Unchecked);
      
        KUrl dir = url;
        QString documentRootHint = url.directory().isEmpty() ? "/" : url.directory();
        dir.setPath(documentRootHint);
        DocumentRootDialog dialog(this, dir);
        dialog.exec();
        search_manager_->setDocumentRoot(dialog.url());
    }
    else if(checkBoxLogin->isChecked())
    {
        if(login_dialog_) {
            delete login_dialog_;
            login_dialog_ = 0;
        }
        login_dialog_ = new HttpPostDialog(current_text, this);
//         login_dialog_->setDomainField(url.host());
//         login_dialog_->setPostUrlField(url.path());
        
        login_dialog_->exec();

        search_manager_->setIsLoginPostRequest(true);
        search_manager_->setPostUrl(login_dialog_->postUrl());
        search_manager_->setPostData(login_dialog_->postData());

        url = KUrl(url, login_dialog_->postUrl());
    }

//     if(!checkbox_recursively->isChecked())
//     {
//         search_manager_->setSearchMode(SearchManager::depth);
//         search_manager_->setDepth(0);
//     }

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
    }
    else
    {
        search_manager_->setCheckExternalLinks(false);
    }
    
    if(!lineedit_reg_exp->text().isEmpty())
    {
        search_manager_->setCheckRegularExpressions(true);
        search_manager_->setRegularExpression(lineedit_reg_exp->text(), false);
    }

    kDebug(23100) <<  "URI to check: " << url.prettyUrl();
    
    combobox_url->setEditText(url.prettyUrl());
    url_to_check_ = url;

    emit signalSearchStarted();

    search_manager_->startSearch(url);

    saveSession();
}

void SessionWidget::keyPressEvent(QKeyEvent* e)
{
    if( (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
         &&
        ( combobox_url->hasFocus()
         || spinbox_depth->hasFocus()
         || checkbox_external_links->hasFocus()
         || checkbox_subdirs_only->hasFocus() ) )
    {
        slotStartSearch();
    }
    else if(e->key() == Qt::Key_F6)
    {
        combobox_url->lineEdit()->selectAll();
    }
    
    QWidget::keyPressEvent(e);
}

bool SessionWidget::validFields()
{
    QString url_string = combobox_url->currentText();
//     kDebug(23100) << "SessionWidget::validFields: " << url_string;
    if(url_string.isEmpty())
    {
        KMessageBox::sorry(this, i18n("Cowardly refusing to check an empty URL."));
        return false;
    }
    return true;
}

QString SessionWidget::title() const
{
    QString label;
    LinkStatus const* linkstatus = search_manager_->linkStatusRoot();
    KUrl url = linkstatus->absoluteUrl();
    
    if(linkstatus->hasHtmlDocTitle())
    {
        label = linkstatus->htmlDocTitle();
        label = KStringHandler::csqueeze(label, 30);
    }
    else
    {
        if(url.fileName(KUrl::ObeyTrailingSlash).isEmpty())
            label = url.prettyUrl();
        else
            label = url.fileName(KUrl::ObeyTrailingSlash);
        
        label = KStringHandler::lsqueeze(label, 30);        
    }

    return label;
}

void SessionWidget::slotRootChecked(LinkStatus* linkstatus)
{
    resultsSearchBar->show();

    ActionManager::getInstance()->action("file_export_html_all")->setEnabled(!isEmpty());
    ActionManager::getInstance()->action("file_export_html_broken")->setEnabled(!isEmpty());
    ActionManager::getInstance()->action("file_create_site_map")->setEnabled(!isEmpty());
    
    emit signalTitleChanged();

    Q_ASSERT(textlabel_progressbar->text() == i18n("Checking...") ||
            textlabel_progressbar->text() == i18n("Stopped"));
    progressbar_checker->setValue(1);

    textedit_checked_links->setText(QString::number(search_manager_->checkedLinks()));

    TreeViewItem* tree_view_item = 0;
    if(check_in_background_) {
        tree_view_item = new TreeViewItem(tree_view, 0, linkstatus);
    }
    else {
        tree_view_item = new TreeViewItem(tree_view, linkstatus);
    }      

    linkstatus->setTreeViewItem(tree_view_item);
}

void SessionWidget::slotLinkChecked(LinkStatus* linkstatus)
{
//     Q_ASSERT(textlabel_progressbar->text() == i18n("Checking...") ||
//             textlabel_progressbar->text() == i18n("Stopped"));
    
    progressbar_checker->setValue(progressbar_checker->value() + 1);
    textedit_checked_links->setText(QString::number(search_manager_->checkedLinks()));

    if(!linkstatus->checked())
        return;

    TreeViewItem* tree_view_item = 0;
    TreeViewItem* parent_item = linkstatus->parent()->treeViewItem();
    bool match = resultsSearchBar->currentLinkMatcher().matches(*linkstatus);

    if(tree_display_)
    {
        tree_view_item = new TreeViewItem(tree_view, parent_item, linkstatus);
        if(parent_item) {
            parent_item->setLastChild(tree_view_item);
        }
        if(follow_last_link_checked_)
            tree_view->ensureRowVisible(tree_view_item, tree_display_);

        tree_view_item->setHidden(!match);

        // update parent items...
        if(match && resultsSearchBar->currentLinkMatcher().status() != LinkStatusHelper::none) {
            tree_view->show(resultsSearchBar->currentLinkMatcher());
        }
    }
    else
    {
        tree_view_item = new TreeViewItem(tree_view, linkstatus);
        if(follow_last_link_checked_)
            tree_view->ensureRowVisible(tree_view_item, tree_display_);

        tree_view_item->setHidden(!match);
    }
    Q_ASSERT(tree_view_item);
    
    linkstatus->setTreeViewItem(tree_view_item);
}

void SessionWidget::slotSearchFinished(SearchManager*)
{
    Q_ASSERT(in_progress_);
    Q_ASSERT(!paused_);
    Q_ASSERT(!stopped_);

    if(check_in_background_) {
        loadResults();
    }

    KApplication::beep ();

    textlabel_progressbar->setText(i18n( "Ready" ));
    progressbar_checker->reset();
    progressbar_checker->setTextVisible(false);
    progressbar_checker->setRange(0, 1);
    progressbar_checker->setValue(0);

    ready_ = true;

    textlabel_elapsed_time->setEnabled(true);
    textedit_elapsed_time_value->setEnabled(true);
    label_checked_links->setEnabled(true);
    textedit_checked_links->setEnabled(true);
    elapsed_time_timer_.stop();

    in_progress_ = false;
    paused_ = false;
    stopped_ = true;
    resetPendingActions();
    emit signalUpdateActions();

    Global::getInstance()->setStatusBarText(i18n("Finished checking %1",
        combobox_url->currentText()), false);

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
    textedit_elapsed_time_value->setEnabled(true);
    label_checked_links->setEnabled(true);
    textedit_checked_links->setEnabled(true);
    elapsed_time_timer_.stop();

    resetPendingActions();
    emit signalUpdateActions();

    Global::getInstance()->setStatusBarText(i18n("Paused"), false);

    emit signalSearchPaused();
}

void SessionWidget::saveSession()
{
    QString current_text = combobox_url->currentText();
    
    insertUrlAtCombobox(current_text);
    combobox_url->saveItems(); // save on disk

    saveSessionSearchOptions();
}

void SessionWidget::slotLoadSession(QString const& /*url*/)
{
    loadSession();
}

void SessionWidget::loadSession()
{
    QString url = combobox_url->currentText();

    QDomElement element;
    Global::getInstance()->findCurrentSession(url, element);

    if(!element.isNull()) {
        SessionTO sessionTO;
        sessionTO.load(element);

        loadSessionTO(sessionTO);
    }
}

void SessionWidget::loadSessionTO(SessionTO const& session)
{
    spinbox_depth->setValue(session.depth);
    checkbox_subdirs_only->setChecked(!session.checkParentFolders);
    checkbox_external_links->setChecked(session.checkExternalLinks);
    lineedit_reg_exp->setText(session.regExp);
    
//     session.login = checkBoxLogin->isChecked();
//     if(session.login && login_dialog_) {
//         session.postUrl = login_dialog_->postUrl();
//         session.postData = login_dialog_->postData();
//     }    
}

SessionTO SessionWidget::buildSessionTO() const
{
    SessionTO session;
    
    session.url = combobox_url->currentText();
    session.depth = spinbox_depth->value();
    session.checkParentFolders = !checkbox_subdirs_only->isChecked();
    session.checkExternalLinks = checkbox_external_links->isChecked();
    session.regExp = lineedit_reg_exp->text();
    
    session.login = checkBoxLogin->isChecked();
    if(session.login && login_dialog_) {
        session.postUrl = login_dialog_->postUrl();
        session.postData = login_dialog_->postData();
    }

    return session;
}

void SessionWidget::saveSessionSearchOptions()
{
    SessionTO session = buildSessionTO();
    
    QDomDocument& doc = Global::getInstance()->sessionsDocument();
    session.save(doc);
    Global::getInstance()->saveSessionsDocument();
}

void SessionWidget::insertUrlAtCombobox(QString const& url)
{
    combobox_url->addToHistory(url);
}

void SessionWidget::slotSetTimeElapsed()
{
    QTime current(0, 0);
    current = current.addMSecs(start_time_.elapsed());
    textedit_elapsed_time_value->setText(current.toString("hh:mm:ss"));
}

void SessionWidget::slotAddingLevel(bool adding)
{
    if(adding) {
        Q_ASSERT(textlabel_progressbar->text() == i18n("Checking..."));
        textlabel_progressbar->setText(i18n("Adding level..."));
    }
    else {
        Q_ASSERT(textlabel_progressbar->text() == i18n("Adding level..."));
        textlabel_progressbar->setText(i18n("Checking..."));
    }
}

void SessionWidget::slotLinksToCheckTotalSteps(int steps)
{
    textlabel_progressbar->setText(i18n( "Checking..." ));
    progressbar_checker->reset();
    progressbar_checker->setRange(0, steps);
    progressbar_checker->setValue(0);
}

void SessionWidget::slotNewLinksToCheck(int numberOfLinks)
{
    progressbar_checker->setMaximum(progressbar_checker->maximum()
        + numberOfLinks);
}

void SessionWidget::slotIncrementLinksToCheckTotalSteps()
{
    slotNewLinksToCheck(1);
}

void SessionWidget::slotChooseUrlDialog()
{
    setUrl(KFileDialog::getOpenUrl());

    loadSession();
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
    kDebug(23100) << "setFollowLastLinkChecked: " << follow;
    follow_last_link_checked_ = follow;
}

void SessionWidget::slotFollowLastLinkChecked()
{
    follow_last_link_checked_ = !follow_last_link_checked_;
}

void SessionWidget::slotDisableUpdatesOnResultsTable(bool checked)
{
    tree_view->setUpdatesEnabled(!checked);
}

void SessionWidget::slotResetSearchOptions()
{
    slotLoadSettings(true);

    combobox_url->clear();
    lineedit_reg_exp->clear();
}

void SessionWidget::slotRecheckVisibleItems()
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
    resetPendingActions();
    ready_ = false;

//     emit signalSearchStarted();

    in_progress_ = true;
    paused_ = false;
    stopped_ = false;

    emit signalUpdateActions();

    QList<LinkStatus*> items = tree_view->getVisibleLinks();
    search_manager_->recheckLinks(items);
}

void SessionWidget::slotRecheckBrokenItems()
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
    resetPendingActions();
    ready_ = false;

//     emit signalSearchStarted();

    in_progress_ = true;
    paused_ = false;
    stopped_ = false;

    emit signalUpdateActions();

    QList<LinkStatus*> items = tree_view->getBrokenLinks();
    search_manager_->recheckLinks(items);
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

    emit signalUpdateActions();
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
        elapsed_time_timer_.start();

//         emit signalSearchStarted();
        slotLoadSettings(isEmpty()); // it seems that KConfigDialogManager is not trigering this slot

        resetPendingActions();

        Global::getInstance()->setStatusBarText(i18n("Resuming"), false);
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

        emit signalUpdateActions();
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
    if(link_matcher.hasCriteria()) {
//         kDebug(23100) << "has criteria";
//         resultsSearchBar->setStyleSheet(QString("border: 1px solid blue"));
//         resultsSearchBar->setBackgroundRole(QPalette::Highlight);
    }
    else {
//         kDebug(23100) << "not has criteria";
//         resultsSearchBar->setStyleSheet(QString(""));
//         resultsSearchBar->setBackgroundRole(QPalette::Window);
    }

    tree_view->show(link_matcher);
}

void SessionWidget::slotExportAsHTML(LinkStatusHelper::Status status)
{
    KUrl url = KFileDialog::getSaveUrl(KUrl(), "text/html", 0, i18n("Export Results as HTML"));

    if(url.isEmpty())
        return;

//     kDebug(23100) << "\n\nXML document represention: \n\n" << search_manager_->toXML();

    QString preferedStylesheet = KLSConfig::preferedStylesheet();
    KUrl styleSheetUrl = KStandardDirs::locate("appdata", preferedStylesheet);
//     KUrl styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
    kDebug(23100) << "Style sheet URL: " << styleSheetUrl.url();
    if(!styleSheetUrl.isValid()) {
        kWarning(23100) << "Style sheet not valid!";
        styleSheetUrl = KStandardDirs::locate("appdata", "styles/results_stylesheet.xsl");
    }

    QString xml(search_manager_->toXML(status));
    kDebug() << endl << xml;
    
    QString html(XSL::transform(xml, styleSheetUrl));
    FileManager::write(html, url);
}

void SessionWidget::slotCreateSiteMap()
{
    KUrl url = KFileDialog::getSaveUrl(KUrl(), "text/xml", 0, i18n("Create XML Site Map"));
    
    if(url.isEmpty())
        return;
    
    //     kDebug(23100) << "\n\nXML document represention: \n\n" << search_manager_->toXML();
    
    QString xml(search_manager_->buildSiteMapXml());
    kDebug() << endl << xml;
    
    FileManager::write(xml, url);
}

void SessionWidget::slotValidateAll()
{
  if(search_manager_->searchProtocol().startsWith("http"))
  {
    KMessageBox::sorry(this, i18n("Use a protocol other than HTTP, e.g. file, ftp, sftp, fish, etc., so the files can be saved."));
  }

/*    QWizard* wizard = new ValidateAllWizard();
  wizard->show();*/
}

void SessionWidget::slotSearchStarted()
{
    textedit_elapsed_time_value->setText(QTime(0, 0).toString("hh:mm:ss"));
    start_time_.start();
    elapsed_time_timer_.start();

    Global::getInstance()->setStatusBarText(i18n("Checking %1", combobox_url->currentText()), false);
}

void SessionWidget::slotLinkRecheck(LinkStatus* ls)
{
    search_manager_->recheckLink(ls);
}

void SessionWidget::slotLinkRechecked(LinkStatus* ls)
{
    if(in_progress_) {
        Q_ASSERT(textlabel_progressbar->text() == i18n("Checking...") ||
                textlabel_progressbar->text() == i18n("Stopped"));
        progressbar_checker->setValue(progressbar_checker->value() + 1);
    }
    
    if(!ls->checked())
        return;

    if(ls->treeViewItem()) {        
        ls->treeViewItem()->refresh(ls);
        tree_view->setCurrentItem(ls->treeViewItem());
    }
    else {
        kError(23100) << "LinkStatus does not have tree view item: " << ls->absoluteUrl() << endl;
        kError(23100) << "Is redirection: " << ls->isRedirection() << endl;
    }
    if(!in_progress_)
        Global::getInstance()->setStatusBarText(i18n("Done rechecking %1", ls->absoluteUrl().prettyUrl()));
}

KUrl const& SessionWidget::urlToCheck() const
{
    return url_to_check_;
}

bool SessionWidget::supportsResuming()
{
    return true;
}

void SessionWidget::loadResults()
{
    Q_ASSERT(check_in_background_);

    LinkStatus const* linkstatus_root = search_manager_->linkStatusRoot();
    QTreeWidgetItem* item = linkstatus_root->treeViewItem();
    tree_view->insertTopLevelItem(0, item);
    tree_view->expandItem(item);
}



QDomDocument SessionTO::save(QDomDocument& _doc) const
{
    QDomNode node = _doc.namedItem("sessions");
    QDomElement sessionsElement;
    if(!node.isNull() && node.isElement()) {
        sessionsElement = node.toElement();
    }
    else {
        return _doc;
    }

    QDomElement element;
    Global::getInstance()->findCurrentSession(url, element);

    if(!element.isNull()) {
        sessionsElement.removeChild(element);
    }

    element = _doc.createElement("session");
    element.setAttribute("url", url);
    sessionsElement.appendChild(element);

    // <url>
//     QDomElement child_element = element.ownerDocument().createElement("url");
//     child_element.appendChild(element.ownerDocument().createTextNode(url));
//     element.appendChild(child_element);

    // <depth>
    QDomElement child_element = element.ownerDocument().createElement("depth");
    child_element.appendChild(element.ownerDocument().
            createTextNode(QString::number(depth)));
    element.appendChild(child_element);

    // <check_parent_folders>
    child_element = element.ownerDocument().createElement("check_parent_folders");
    child_element.appendChild(element.ownerDocument().
            createTextNode(checkParentFolders ? "true" : "false"));
    element.appendChild(child_element);

    // <check_external_links>
    child_element = element.ownerDocument().createElement("check_external_links");
    child_element.appendChild(element.ownerDocument().
            createTextNode(checkExternalLinks ? "true" : "false"));
    element.appendChild(child_element);

    // <check_regular_expression>
    child_element = element.ownerDocument().createElement("check_regular_expression");
    child_element.appendChild(element.ownerDocument().
            createTextNode(regExp));
    element.appendChild(child_element);
    
    if(login) {
        // <login>
        child_element = element.ownerDocument().createElement("login");
        element.appendChild(child_element);

        QDomElement postUrlElement = element.ownerDocument().createElement("postUrl");
        child_element.appendChild(postUrlElement);
        postUrlElement.appendChild(element.ownerDocument().
                createTextNode(postUrl));

        QDomElement postDataElement = element.ownerDocument().createElement("postData");
        child_element.appendChild(postDataElement);
        postDataElement.appendChild(element.ownerDocument().
                createTextNode(postData));
    }

    return _doc;
}

void SessionTO::load(QDomElement const& sessionElement)
{
    url = sessionElement.attribute("url");
  
//     QDomNode node = sessionElement.namedItem("url");
//     if(!node.isNull() && node.isElement()) {
//         url = node.toElement().text();
//     }
    
    QDomNode node = sessionElement.namedItem("depth");
    if(!node.isNull() && node.isElement()) {
        depth = node.toElement().text().toInt();
    }
    
    node = sessionElement.namedItem("check_parent_folders");
    if(!node.isNull() && node.isElement()) {
        checkParentFolders = node.toElement().text() == "true" ? true : false;
    }
        
    node = sessionElement.namedItem("check_external_links");
    if(!node.isNull() && node.isElement()) {
        checkExternalLinks = node.toElement().text() == "true" ? true : false;
    }
        
    node = sessionElement.namedItem("check_regular_expression");
    if(!node.isNull() && node.isElement()) {
        regExp = node.toElement().text();
    }
    
    node = sessionElement.namedItem("login");
    if(!node.isNull() && node.isElement()) {
        login = true;

        QDomNode childNode = node.namedItem("postUrl");
        if(!childNode.isNull() && childNode.isElement()) {
            postUrl = childNode.toElement().text();
        }
        
        childNode = node.namedItem("postData");
        if(!childNode.isNull() && childNode.isElement()) {
            postData = childNode.toElement().text().toUtf8();
        }
    }
}




#include "sessionwidget.moc"
