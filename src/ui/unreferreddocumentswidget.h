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
    
#ifndef UNREFERRED_DOCUMENTS_WIDGET_H
#define UNREFERRED_DOCUMENTS_WIDGET_H

#include <global.h>
#include <kio/job.h>
class KUrl;
class KToggleAction;
class KJob;

#include <QWidget>
#include <QTimer>
class QMenu;

#include "ui_unreferreddocumentswidget.h"
#include "ui/widgetinterface.h"
#include "engine/searchmanager.h"
                      

class UnreferredDocumentsWidget : public PlayableWidgetInterface
{
    Q_OBJECT
public:
    UnreferredDocumentsWidget(KUrl const& baseDirectory, SearchManager const& searchManager, QWidget* parent = 0);
    virtual ~UnreferredDocumentsWidget();

    void setBaseDirectory(KUrl const& url);

    virtual bool supportsResuming();

    virtual void slotStartSearch();
    virtual void slotPauseSearch();
    virtual void slotStopSearch();

private Q_SLOTS:
    void slotChooseUrlDialog();

    void slotEntries(KIO::Job*, const KIO::UDSEntryList&);
    void slotPercent(KJob* job, unsigned long percent);
    void slotResult(KJob*);

    void slotUnreferredDocStepCompleted();
    void slotUnreferredDocFound(const QString& doc);

    void slotDeleteCheckedDocuments();
    void slotDeleteAllDocuments();
    void slotResultItemsDeleted(KJob*);

    void slotPopupDocumentListPopup(const QPoint& point);

private:
    void init();
    void finish();
    void deleteDocuments(bool onlyChecked = false);

    static KUrl normalizeBaseDirectoryInput(QString const& input);

private:
    Ui::UnreferredDocumentsWidget m_ui;
    KUrl m_baseDirectory;
    QStringList m_documentList;
    SearchManager const& m_searchManager;
    QTimer m_elapsedTimeTimer;
    KToggleAction* m_startSearchAction;
    QMenu* m_documentListPopup;
};
    
#endif
