/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
 *   kde@mouraguedes.com                                                   *
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

#ifndef AUTOMATIONENGINE_H
#define AUTOMATIONENGINE_H

#include <QObject>
#include <QString>

#include "klinkstatus_export.h"
class SearchManager;


class KLINKSTATUS_EXPORT AutomationEngine : public QObject
{
    Q_OBJECT
public:
    AutomationEngine(QObject* parent = 0);
    ~AutomationEngine();

    void startLinkCheck();

    QString configurationFilesDir() const;
    void setConfigurationFilesDir(QString const& dir);

signals:
    void signalSearchFinished();

private:
    void check(QString const& file);
    
private:
    QString m_configurationFilesDir;
};


inline AutomationEngine::AutomationEngine(QObject* parent)
  : QObject(parent)
{
}

inline AutomationEngine::~AutomationEngine()
{
}

inline QString AutomationEngine::configurationFilesDir() const
{
    return m_configurationFilesDir;
}

inline void AutomationEngine::setConfigurationFilesDir(QString const& dir)
{
    m_configurationFilesDir = dir;
}


#endif
