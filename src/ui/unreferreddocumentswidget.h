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

class KUrl;

#include <QWidget>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QTimer>
        
#include "ui_unreferreddocumentswidget.h"
    

class UnreferredDocumentsWidget : public QWidget
{
    Q_OBJECT
public:
    UnreferredDocumentsWidget(QString const& baseDirectory, QWidget* parent = 0);
    ~UnreferredDocumentsWidget();

    void setBaseDirectory(KUrl const& url);

private Q_SLOTS:
    void slotChooseUrlDialog();
    
private:
    void init();
    
private:
    Ui::UnreferredDocumentsWidget m_ui;
    QString m_baseDirectory;
    QTimer m_elapsedTimeTimer;

    QStringListModel m_listModel;
    QSortFilterProxyModel m_proxyModel;
};
    
#endif
