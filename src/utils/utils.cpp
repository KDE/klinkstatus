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

#include "utils.h"

#include <qprocess.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <kdebug.h>


QString htmlDocCharset[NUMBER_OF_HTML_CODES][2] = {

	{ "&euro;", "@" },
	{ "&#09;", "\t" },
	{ "&#10;", "\n" },
	{ "&#13;", "\r" },
	{ "&#32;", " " },
	{ "&#33;", "!" },
	{ "&#34;", "\"" },
	{ "&#35;", "#" },
	{ "&#36;", "$" },
	{ "&#37;", "%" },
	{ "&#38;", "&" },
	{ "&#39;", "'" },
	{ "&#40;", "(" },
	{ "&#41;", ")" },
	{ "&#42;", "*" },
	{ "&#43;", "+" },
	{ "&#44;", "," },
	{ "&#45;", "-" },
	{ "&#46;", "." },
	{ "&#47;", "/" },
	// numbers....
	{ "&#58;", ":" },
	{ "&#59;", ";" },
	{ "&#60;", "<" },
	{ "&#61;", "=" },
	{ "&#62;", ">" },
	{ "&#63;", "?" },
	{ "&#64;", "@" },
	// letters...
	{ "&#91;", "[" },
	{ "&#92;", "\\" },
	{ "&#93;", "]" },
	{ "&#94;", "^" },
	{ "&#95;", "_" },
	{ "&#96;", "`" },
	//letters...
	{ "&#123;", "{" },
	{ "&#124;", "|" },
	{ "&#125;", "}" },
	{ "&#126;", "~" },
	{ "&#128;", "?" },
	{ "&#130;", "," },
	{ "&#131;", "?" },
	{ "&#132;", "\"" },
	{ "&#133;", "?" },
	{ "&#134;", "?" },
	{ "&#135;", "?" },
	{ "&#137;", "?" },
	{ "&#138;", "?" },
	{ "&#139;", "<" },
	{ "&#140;", "?" },
	{ "&#142;", "?" },
	{ "&#145;", "'" },
	{ "&#146;", "'" },
	{ "&#147;", "\"" },
	{ "&#148;", "\"" },
	{ "&#149;", "*" },
	{ "&#150;", "-" },
	{ "&#151;", "-" },
	{ "&#152;", "~" },
	{ "&#153;", "?" },
	{ "&#154;", "?" },
	{ "&#155;", ">" },
	{ "&#156;", "?" },
	{ "&#158;", "?" },
	{ "&#159;", "?" },
	{ "&#161;", "?" },
	{ "&#162;", "?" },
	{ "&#163;", "?" },
	{ "&#164;", "?" },
	{ "&#165;", "?" },
	{ "&#166;", "?" },
	{ "&#167;", "?" },
	{ "&#168;", "?" },
	{ "&#169;", "" },
	{ "&#170;", "?" },
	{ "&#171;", "?" },
	{ "&#172;", "?" },
	{ "&#174;", "?" },
	{ "&#175;", "?" },
	{ "&#176;", "" },
	{ "&#177;", "?" },
	{ "&#178;", "" },
	{ "&#179;", "?" },
	{ "&#180;", "?" },
	{ "&#181;", "?" },
	{ "&#182;", "?" },
	{ "&#183;", "" },
	{ "&#184;", "?" },
	{ "&#185;", "?" },
	{ "&#186;", "?" },
	{ "&#187;", "?" },
	{ "&#188;", "?" },
	{ "&#189;", "?" },
	{ "&#190;", "?" }

};


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

namespace FileManager
{
QString read(QString const& path)
{
    QFile file(path);

    if(!file.open(IO_ReadOnly))
    {
        kdDebug() << "File " << path << " not found." << endl;
        return QString();
    }

    QTextStream stream(&file);
    QString fileString = stream.read();

    file.close();

    return fileString;
}
}
