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

#include "scriptingmodule.h"

#include <kdemacros.h>

#include "klinkstatusbasepart.h"
#include "ui/view.h"
#include "engine/searchmanager.h"
#include "engine/searchmanageragent.h"
#include "interfaces/engine/isearchmanager.h"
#include "interfaces/ui/iview.h"


extern "C" {
KDE_EXPORT QObject* krossmodule()
{
    return new ScriptingModule(0);
}
}


class ScriptingModule::Private
{
public:
};

ScriptingModule::ScriptingModule(QObject* parent)
    : QObject(parent)
    , d(new Private())
{
    setObjectName("KLinkStatus");
}


ScriptingModule::~ScriptingModule()
{
    delete d;
}

KLinkStatusBasePart* ScriptingModule::basePart() const
{
    if(!parent()) {
        kDebug() << "this->parent() is null";
        return 0;
    }

    KLinkStatusBasePart* part = dynamic_cast<KLinkStatusBasePart*> (parent());
    if(!part) {
        kDebug() << "ScriptingModule::view - parent is not a KLinkStatusBasePart";
        kDebug() << parent()->metaObject()->className();
        return 0;
    }
    
    return part;
}

QObject* ScriptingModule::view()
{
    KLinkStatusBasePart* part = basePart();
    if(!part) {
        return 0;
    }

    View* view = part->view();
    return view->findChild<IView*> ();
}

// QObject* ScriptingModule::searchManagerAgent()
// {
//     KLinkStatusBasePart* part = basePart();
// 
//     SearchManagerAgent* agent = part->searchManagerAgent();
//     return agent->findChild<SearchManagerAgent*> ();
// }


#include "scriptingmodule.moc"
