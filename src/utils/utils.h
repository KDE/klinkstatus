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

#ifndef UTILS_H
#define UTILS_H

#include <kurl.h>
#include <qstring.h>

//#include <string>
#include <cassert>

using namespace std;


int const NUMBER_OF_HTML_CODES = 92;

QString const htmlDocCharset[NUMBER_OF_HTML_CODES][2] = {

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

/**
  Decode the html charset.
  e.g.
  decode("mail&#64;server&#46;org") => "mail@server.org"
*/
void decode(QString& url);
//void decode(string& url);

/**
   Compares to integers and returns -1 if a is smaller than b,
   1 if b is smaller than a, and 0 if a and b are equal or both negative.
   If one of the integers is negative and the other isn't, it is considered
   that the positive is smaller.
   e.g.:
   a =  0, b = +1 => -1
   a = +1, b =  0 => +1
   a = -1, b = -1 =>  0
   a = +3, b = +3 =>  0
   a = +1, b = -1 => -1
*/
int smallerUnsigned(int a, int b);

class QWidget;
//void viewUrlInBrowser(KURL const& url, QWidget* parent, QString browser = "mozilla");


#endif
