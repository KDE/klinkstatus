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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "klinkstatus.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <iostream>
using namespace std;


static const char description[] =
    I18N_NOOP("A Link Checker");

static const char version[] = "0.1.3";

static KCmdLineOptions options[] =
    {
        { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
        KCmdLineLastOption
    };

int main(int argc, char *argv[])
{
    KAboutData about("klinkstatus", I18N_NOOP("KLinkStatus"), version, description,
                     KAboutData::License_GPL_V2, "(C) 2004 Paulo Moura Guedes", 0,
                     "http://kde-apps.org/content/show.php?content=12318");

    about.addAuthor("Paulo Moura Guedes", 0, "moura@kdewebdev.org");

    about.addCredit("Manuel Menezes de Sequeira", 0, 0, "http://home.iscte.pt/~mms/");
    about.addCredit("Gonçalo Silva", 0, "gngs@paradigma.co.pt");
    about.addCredit("Nuno Monteiro", 0, 0, "http://www.itsari.org");
    about.addCredit("Eric Laffoon", 0, "sequitur@kde.org");
    about.addCredit("Andras Mantia", 0, "amantia@kde.org");
    about.addCredit("Michal Rudolf", 0, "mrudolf@kdewebdev.org");
    about.addCredit("Mathieu Kooiman", 0, " quanta@map-is.nl");
    about.addCredit("Jens Herden", 0, "jens@kdewebdev.org");
    about.addCredit("Helge Hielscher", 0, "hhielscher@unternehmen.com");

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;

    // see if we are starting with session management
    if (app.isRestored())
    {
        RESTORE(KLinkStatus);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        KLinkStatus *widget = new KLinkStatus;
        widget->show();

        if ( args->count() == 0 )
        {
            widget->load(KURL());
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++ )
            {
                widget->load( args->url( i ) );
            }
        }
        args->clear();
    }

    return app.exec();
}
