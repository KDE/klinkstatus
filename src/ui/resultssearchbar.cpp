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

#include "resultssearchbar.h"

#include <kcombobox.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <KIcon>

#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QToolTip>
#include <QLayout>
#include <QHBoxLayout>

class ResultsSearchBar::ResultsSearchBarPrivate
{
public:
    ResultsSearchBarPrivate()
    : layout(0), searchLine(0), searchCombo(0), delay(400), m_lastComboIndex(0)
    {}

    QString searchText;
    QTimer timer;
    QHBoxLayout* layout;
    KLineEdit* searchLine;
    KComboBox* searchCombo;
    int delay;
    int m_lastComboIndex;
};

ResultsSearchBar::ResultsSearchBar(QWidget* parent)
        : QFrame(parent), d(new ResultsSearchBar::ResultsSearchBarPrivate)
{
//     setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
//     setAutoFillBackground(true);
    setFrameStyle(QFrame::StyledPanel);

    d->layout = new QHBoxLayout(this);
    d->layout->setMargin(2);
    d->layout->setSpacing(5);

    int const minimumHeight = 31;
//     this->setMinimumHeight(minimumHeight);
    
    QToolButton* clearButton = new QToolButton(this);
    clearButton->setIcon(KIcon(QApplication::layoutDirection() == Qt::RightToLeft ?
        "edit-clear-locationbar-rtl" : "edit-clear-locationbar-ltr"));
    clearButton->setAutoRaise(true);
    clearButton->setMinimumHeight(minimumHeight);
    d->layout->addWidget(clearButton);

    QLabel* searchLabel = new QLabel(this);
    searchLabel->setText(i18n("S&earch:"));
    searchLabel->setMinimumHeight(minimumHeight);
    d->layout->addWidget(searchLabel);

    d->searchLine = new KLineEdit(this);
    d->searchLine->setMinimumHeight(minimumHeight);
    d->layout->addWidget(d->searchLine);

    connect(d->searchLine, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotSearchStringChanged(const QString &)));

    searchLabel->setBuddy(d->searchLine);

    QLabel* statusLabel = new QLabel(this);
    statusLabel->setText(i18n("Status:"));
    statusLabel->setMinimumHeight(minimumHeight);
    d->layout->addWidget(statusLabel);

    d->searchCombo = new KComboBox(this);
    d->searchCombo->setMinimumHeight(minimumHeight);
    QPixmap iconAll = SmallIcon("system-run");
    QPixmap iconGood = SmallIcon("dialog-ok");
    // "ok" icon should probably be "dialog-success", but we don't have that icon in KDE 4.0
    QPixmap iconBroken = SmallIcon("dialog-error");
    QPixmap iconMalformed = SmallIcon("dialog-warning");
    QPixmap iconUndetermined = SmallIcon("unknown");

    d->searchCombo->addItem(iconAll, i18n("All Links"));
    d->searchCombo->addItem(iconGood, i18n("Good Links"));
    d->searchCombo->addItem(iconBroken, i18n("Broken Links"));
    d->searchCombo->addItem(iconMalformed, i18n("Malformed Links"));
    d->searchCombo->addItem(iconUndetermined, i18n("Undetermined Links"));
    d->layout->addWidget(d->searchCombo);

    setLayout(d->layout);

    clearButton->setToolTip( i18n("Clear filter"));
    d->searchLine->setToolTip( i18n("Enter the terms to filter the result link list"));
    d->searchCombo->setToolTip( i18n("Choose what kind of link status to show in result list"));

    connect(clearButton, SIGNAL( clicked() ),
            this, SLOT(slotClearSearch()) );

    connect(d->searchCombo, SIGNAL(activated(int)),
            this, SLOT(slotSearchComboChanged(int)));

    connect(&(d->timer), SIGNAL(timeout()), this, SLOT(slotActivateSearch()));
}

ResultsSearchBar::~ResultsSearchBar()
{
    delete d;
    d = 0;
}

QString const& ResultsSearchBar::text() const
{
    return d->searchText;
}

int ResultsSearchBar::status() const
{
    return d->searchCombo->currentIndex();
}

void ResultsSearchBar::setDelay(int ms)
{
    d->delay = ms;
}

int ResultsSearchBar::delay() const
{
    return d->delay;
}

void ResultsSearchBar::slotClearSearch()
{
    if(status() != 0 || !d->searchLine->text().isEmpty())
    {
        d->searchLine->clear();
        d->searchCombo->setCurrentIndex(0);
        d->timer.stop();
        slotActivateSearch();
    }
}

void ResultsSearchBar::slotSetStatus(int status)
{
    d->searchCombo->setCurrentIndex(status);
}

void ResultsSearchBar::slotSetText(const QString& text)
{
    d->searchLine->setText(text);
}

void ResultsSearchBar::slotSearchComboChanged(int index)
{
    if(d->timer.isActive())
        d->timer.stop();

    if(d->m_lastComboIndex == index)
        return;
    
    d->m_lastComboIndex = index;
    
    d->timer.start(200);
}

void ResultsSearchBar::slotSearchStringChanged(const QString& search)
{
    if(d->timer.isActive())
        d->timer.stop();

    if(d->searchText == search) 
        return;
    
    d->searchText = search;
    
    d->timer.start(200);
}

void ResultsSearchBar::slotActivateSearch()
{
    kDebug(23100) << "ResultsSearchBar::slotActivateSearch";
    
    d->timer.stop();
    
    LinkStatusHelper::Status status = selectedStatus();
    emit signalSearch(LinkMatcher(d->searchLine->text(), status));
}

LinkMatcher ResultsSearchBar::currentLinkMatcher() const
{
    return LinkMatcher(d->searchLine->text(), selectedStatus());
}

LinkStatusHelper::Status ResultsSearchBar::selectedStatus() const
{
    LinkStatusHelper::Status status = LinkStatusHelper::none;
    
    if(d->searchCombo->currentIndex())
    {
        switch(d->searchCombo->currentIndex())
        {
            case 1: // Good
            {
                status = LinkStatusHelper::good;
                break;
            }
            case 2: // Broken
            {
                status = LinkStatusHelper::bad;
                break;
            }
            case 3: // Malformed
            {
                status = LinkStatusHelper::malformed;
                break;
            }
            case 4: // Undetermined
            {
                status = LinkStatusHelper::undetermined;
                break;
            }
            default:
                break;
        }
    }
    return status;
}


#include "resultssearchbar.moc"
