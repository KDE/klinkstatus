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

#include "unreferreddocumentswidget.h"

#include <KFileDialog>
#include <KToggleAction>
#include <KApplication>
#include <KMessageBox>

#include "actionmanager.h"


UnreferredDocumentsWidget::UnreferredDocumentsWidget(KUrl const& baseDir, 
    SearchManager const& searchManager, QWidget* parent)
    : PlayableWidgetInterface(parent), m_baseDirectory(baseDir),
      m_searchManager(searchManager), m_startSearchAction(0)
{
    init();
}

UnreferredDocumentsWidget::~UnreferredDocumentsWidget()
{
}

void UnreferredDocumentsWidget::init()
{
    m_ui.setupUi(this);

    m_ui.baseDirCombo->init();
    m_ui.documentListWidget->setUniformItemSizes(true);

    setBaseDirectory(m_baseDirectory);

    connect(&m_searchManager, SIGNAL(signalUnreferredDocStepCompleted()),
            this, SLOT(slotUnreferredDocStepCompleted()));
    connect(&m_searchManager, SIGNAL(signalUnreferredDocFound(const QString&)),
            this, SLOT(slotUnreferredDocFound(const QString&)));
  
    m_ui.pickUrlButton->setIcon(KIcon("document-open"));
    const int pixmapSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    m_ui.pickUrlButton->setFixedSize(pixmapSize + 8, pixmapSize + 8);
    connect(m_ui.pickUrlButton, SIGNAL(clicked()), this, SLOT(slotChooseUrlDialog()));

    m_elapsedTimeTimer.setInterval(1000);

    m_startSearchAction= static_cast<KToggleAction*> (ActionManager::getInstance()->action("start_search"));
    
    m_ui.documentSearchLine->setListWidget(m_ui.documentListWidget);    
}

void UnreferredDocumentsWidget::slotChooseUrlDialog()
{
    setBaseDirectory(KFileDialog::getExistingDirectoryUrl(m_baseDirectory, this));
}

void UnreferredDocumentsWidget::setBaseDirectory(KUrl const& url)
{
    m_baseDirectory = url;
  
    m_ui.baseDirCombo->addCurrentItem(url.prettyUrl());
    m_ui.baseDirCombo->setFocus();
}

KUrl UnreferredDocumentsWidget::normalizeBaseDirectoryInput(QString const& input)
{
    KUrl url(input);
    Q_ASSERT(!url.protocol().startsWith("http"));

    url.setFileName(QString());
    return url;
}

bool UnreferredDocumentsWidget::supportsResuming()
{
    return false;
}

void UnreferredDocumentsWidget::slotStartSearch()
{
    if(in_progress_) {
        m_startSearchAction->setChecked(true); // do not toggle
        Q_ASSERT(!ready_);
        KApplication::beep();
        return;
    }
    Q_ASSERT(ready_);
    
    ready_ = false;
    in_progress_ = true;

    QString inputBaseDir = m_ui.baseDirCombo->currentText();
    
    if(inputBaseDir.startsWith("http")) {
        KMessageBox::sorry(this, i18n("Cannot crawl through directories using HTTP.\n\nTry using file, ftp, sftp or fish, for example."));
        return;
    }

    m_baseDirectory = normalizeBaseDirectoryInput(inputBaseDir);
    m_ui.baseDirCombo->setEditText(m_baseDirectory.url());

    m_ui.progress->reset();
    m_ui.progress->setRange(0, 100);

    m_ui.progressLabel->setText(i18n("Crawling folders..."));
    
    KIO::ListJob* job = KIO::listRecursive(m_baseDirectory, false, false);
    
    connect(job, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
            this, SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&)));
    connect(job, SIGNAL(percent(KJob*, unsigned long)),
            this, SLOT(slotPercent(KJob*, unsigned long)));
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

void UnreferredDocumentsWidget::slotPauseSearch()
{
}

void UnreferredDocumentsWidget::slotStopSearch()
{
}

void UnreferredDocumentsWidget::slotEntries(KIO::Job* /*job*/, const KIO::UDSEntryList& list)
{
    foreach(KIO::UDSEntry entry, list) {
        if(entry.isDir())
            continue;

        m_documentList << entry.stringValue(KIO::UDS_NAME);
    }
}

void UnreferredDocumentsWidget::slotPercent(KJob*, unsigned long percent)
{
    m_ui.progress->setValue(percent);
} 

void UnreferredDocumentsWidget::slotResult(KJob*)
{
    m_ui.progress->reset();

    if(m_documentList.size() == 0) {
        m_ui.progressLabel->setText(i18n("Ready"));
        return;
    }
    
    m_ui.progress->setMaximum(m_documentList.size() - 1);

    m_ui.progressLabel->setText(i18n("Matching results..."));

    m_searchManager.findUnreferredDocuments(m_baseDirectory, m_documentList);
    finish();    
}

void UnreferredDocumentsWidget::finish()
{
    ready_ = true;
    in_progress_ = false;
    
    m_ui.progressLabel->setText(i18n("Ready"));
    m_ui.progress->reset();
    m_startSearchAction->setChecked(false);
}

void UnreferredDocumentsWidget::slotUnreferredDocStepCompleted()
{
    m_ui.progress->setValue(m_ui.progress->value() + 1);
}

void UnreferredDocumentsWidget::slotUnreferredDocFound(const QString& document)
{
    QListWidgetItem* item = new QListWidgetItem(document, m_ui.documentListWidget);
    item->setFlags(Qt::ItemIsEnabled
        | Qt::ItemIsSelectable
        | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
}

#include "unreferreddocumentswidget.moc"
