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

#include "scriptingpart.h"

#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kcmdlineargs.h>
#include <kurl.h>
#include <klocale.h>

#include "scriptingmodule.h"

typedef KGenericFactory<ScriptingPart> KLinkStatusScriptingFactory;
K_EXPORT_COMPONENT_FACTORY(krossmoduleklinkstatus, KLinkStatusScriptingFactory("krossmoduleklinkstatus"))

class ScriptingPart::Private
{
public:
    QPointer<ScriptingModule> module;
};

ScriptingPart::ScriptingPart(QObject* parent, const QStringList&)
    : Kross::ScriptingPlugin(parent)
    , d(new Private())
{
    d->module = new ScriptingModule(parent);

    setComponentData(ScriptingPart::componentData());
    setXMLFile(KStandardDirs::locate("data", "klinkstatus/kpartplugins/scripting.rc"), true);
    kDebug(23100) <<"Scripting plugin. Class:" << metaObject()->className() 
        <<", Parent:" << parent->metaObject()->className();
    
//     QString ss;
//     QTextStream s(&ss);
//     QDomDocument doc = domDocument();
//     doc.save(s, 4);
//     kDebug(23100) << s.readAll();

    initActions();
}

ScriptingPart::~ScriptingPart()
{
    delete d;
}

void ScriptingPart::initActions()
{
    KAction* action  = new KAction(i18n("Edit Script Actions..."), this);
    actionCollection()->addAction("edit_script_actions", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotEditScriptActions()));

    action  = new KAction(i18n("Reset Script Actions..."), this);
    actionCollection()->addAction("reset_script_actions", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotResetScriptActions()));
}

#include "scriptingpart.moc"
