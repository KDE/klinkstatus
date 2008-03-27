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

#ifndef SCRIPTINGPART_H
#define SCRIPTINGPART_H

#include <kparts/plugin.h>

class SearchManagerAgent;

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class AutomationPart : public KParts::Plugin
{
    Q_OBJECT
public:
    explicit AutomationPart(QObject* parent, const QStringList& list);
    ~AutomationPart();
    
private Q_SLOTS:
    void slotConfigureLinkChecks();
    void slotTimeout(QObject*);

private:
    void initActions();
    void initLinkChecks();
    
    void scheduleCheck(QString const& configurationFilePath);

private:
    Q_DISABLE_COPY(AutomationPart)

    class Private;
    Private* const d;
};


#endif
