/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   pmg@netcabo.pt                                                        *
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

#include "utils.h"

#include <qprocess.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kmessagebox.h>


void decode(QString& url)
{
    if( (int)url.find('&') != -1)
    {
        for(int i = 0; i != NUMBER_OF_HTML_CODES; ++i)
        {
            int index = url.find(htmlDocCharset[i][0]);
            if(index != - 1)
            {
                url.replace(htmlDocCharset[i][0], htmlDocCharset[i][1]);
            }
        }
    }
}
/*
void decode(string& url)
{
    if( (int)url.find('&') != -1)
    {
        for(int i = 0; i != NUMBER_OF_HTML_CODES; ++i)
        {
            int index = url.find(htmlDocCharset[i][0].latin1());
            if(index != - 1)
            {
                int length = htmlDocCharset[i][0].length();
                url.replace(index, length, htmlDocCharset[i][1].latin1());
            }
        }
    }
}
*/
int smallerUnsigned(int a, int b)
{
    if(a >= 0 && b >= 0)
    {
        if(a < b)
            return -1;
        else if(a > b)
            return 1;
        else
            return 0;
    }

    else if(a < 0 && b < 0)
        return 0;

    else if(a < 0)
        return 1;

    else
        return -1;
}
/*
void viewUrlInBrowser(KURL const& url, QWidget* parent, QString browser)
{
    assert(url.isValid());

#ifdef Q_WS_WIN

    HINSTANCE status =::ShellExecute( KApplication::kApplication()->mainWidget()->winId(), NULL,
                                      ( TCHAR * ) qt_winTchar( url.prettyURL(), true ), NULL, NULL, SW_SHOW );
#else

    QProcess* process = new QProcess( parent, "process_browser" );
    process->clearArguments();
    process->addArgument(browser);

    process->addArgument(url.prettyURL());

    if( !process->start() )
    {
        KMessageBox::critical(parent, "Error - KLinkStatus", "Can't open Browser",
                              KMessageBox::Ok | KMessageBox::Default, KMessageBox::NoButton);
    }
#endif
}
*/
