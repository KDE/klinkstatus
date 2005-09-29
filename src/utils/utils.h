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

#ifndef UTILS_H
#define UTILS_H

#include <kurl.h>
#include <qstring.h>

//#include <string>


using namespace std;


int const NUMBER_OF_HTML_CODES = 92;
extern QString htmlDocCharset[NUMBER_OF_HTML_CODES][2];

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

namespace FileManager
{
QString read(QString const& path);
}


#endif
