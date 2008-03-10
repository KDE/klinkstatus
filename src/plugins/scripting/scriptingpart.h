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

#include <kross/ui/plugin.h>

class ScriptingModule;


/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class ScriptingPart : public Kross::ScriptingPlugin
{
public:
    ScriptingPart(QObject* parent, const QStringList& list);
    ~ScriptingPart();

    ScriptingModule* module() const;

private:
    void initActions();

private:
    Q_DISABLE_COPY(ScriptingPart)

    class Private;
    Private* const d;
};

#endif
