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

#include "klinkstatus.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>


static const char description[] =
    I18N_NOOP("A Link Checker.\n\nKLinkStatus belongs to the kdewebdev module from KDE.");

static const char version[] = "0.3.92";

int main(int argc, char *argv[])
{
    KAboutData about("klinkstatus", 0, ki18n("KLinkStatus"), version, ki18n(description),
                     KAboutData::License_GPL_V2, ki18n("(C) 2004-2007 Paulo Moura Guedes"), KLocalizedString(),
                     "http://klinkstatus.kdewebdev.org");

    about.addAuthor(ki18n("Paulo Moura Guedes"), KLocalizedString(), "moura@kdewebdev.org");

    about.addCredit(ki18n("Manuel Menezes de Sequeira"), KLocalizedString(), 0, "http://home.iscte.pt/~mms/");
    about.addCredit(ki18n("Gonçalo Silva"), KLocalizedString(), "http://paradigma.pt/gngs/");
    about.addCredit(ki18n("Nuno Monteiro"), KLocalizedString(), 0, "http://www.itsari.org");
    about.addCredit(ki18n("Eric Laffoon"), KLocalizedString(), "sequitur@kde.org");
    about.addCredit(ki18n("Andras Mantia"), KLocalizedString(), "amantia@kde.org");
    about.addCredit(ki18n("Michal Rudolf"), KLocalizedString(), "mrudolf@kdewebdev.org");
    about.addCredit(ki18n("Mathieu Kooiman"), KLocalizedString(), " quanta@map-is.nl");
    about.addCredit(ki18n("Jens Herden"), KLocalizedString(), "jens@kdewebdev.org");
    about.addCredit(ki18n("Helge Hielscher"), KLocalizedString(), "hhielscher@unternehmen.com");

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n("Document to open"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(KLinkStatus);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

        KLinkStatus *widget = new KLinkStatus;        
        widget->show();

        if ( args->count() == 0 )
        {
            widget->load(KUrl());
        }
        else
        {
            for(int i = 0; i != args->count(); ++i)
            {
                widget->load(args->url(i));
            }
        }
        args->clear();
    }

    return app.exec();
}
