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

UnreferredDocumentsWidget::UnreferredDocumentsWidget(QString const& baseDir, QWidget* parent)
    : QWidget(parent), m_baseDirectory(baseDir)
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

    m_ui.pickUrlButton->setIcon(KIcon("document-open"));
    const int pixmapSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    m_ui.pickUrlButton->setFixedSize(pixmapSize + 8, pixmapSize + 8);
    connect(m_ui.pickUrlButton, SIGNAL(clicked()), this, SLOT(slotChooseUrlDialog()));

    m_elapsedTimeTimer.setInterval(1000);


    m_proxyModel.setSourceModel(&m_listModel);
    m_ui.listView->setModel(&m_proxyModel);

    connect(m_ui.filterLineEdit, SIGNAL(textChanged(const QString&)),
            &m_proxyModel, SLOT(slotFilterRegExp(const QString&)));
}

void UnreferredDocumentsWidget::slotChooseUrlDialog()
{
    setBaseDirectory(KFileDialog::getOpenUrl());
}

void UnreferredDocumentsWidget::setBaseDirectory(KUrl const& url)
{
    m_ui.baseDirCombo->addCurrentItem(url.prettyUrl());
    m_ui.baseDirCombo->setFocus();
}



#include "unreferreddocumentswidget.moc"
