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

#ifndef ISEARCH_MANAGER
#define ISEARCH_MANAGER

#include <QObject>

#include <KUrl>

#include <QSet>

class SearchManager;


class ISearchManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kdewebdev.klinkstatus.ISearchManager")

public:
    ISearchManager(QObject* parent);

public Q_SLOTS:
    void checkAndExportToHtml(QString const& optionsFilePath);
//     void checkAndMailResuls(QString const& optionsFilePath);

private Q_SLOTS:
    void slotExportSearchFinished(SearchManager* searchManager);
    void slotEmailSearchFinished(SearchManager* searchManager);

private:
    void initSearchManager(SearchManager* searchManager, QString const& optionsFilePath);

private:
    QSet<SearchManager*> m_exportSearchManagerList;
    QSet<SearchManager*> m_mailSearchManagerList;

    KUrl m_exportResultsPath;
};

#endif
