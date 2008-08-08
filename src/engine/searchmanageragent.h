/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
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

#ifndef SEARCHMANAGERAGENT_H
#define SEARCHMANAGERAGENT_H

#include <QObject>

#include "klinkstatus_export.h"
class SearchManager;


/**
 * This class performs higher level operations over SearchManager.
 */
class KLINKSTATUS_EXPORT SearchManagerAgent : public QObject
{
  Q_OBJECT
public:
    SearchManagerAgent(QObject *parent = 0);    
    ~SearchManagerAgent();
    
    QString const& optionsFilePath() const;
    void setOptionsFilePath(QString const& optionsFilePath);

    void check();
    void check(QString const& optionsFilePath);
    
    SearchManager* searchManager() const;

signals:
    void signalSearchFinished(SearchManager* searchManager);
    
private Q_SLOTS:
    void slotExportSearchFinished(SearchManager* searchManager);
    
private:
    void reset();
    bool initSearchOptions(SearchManager* searchManager);
    
private:
    class SearchManagerAgentPrivate;
    SearchManagerAgentPrivate* const d;
};

#endif
