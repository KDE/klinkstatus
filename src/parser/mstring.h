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

#ifndef STRING_H
#define STRING_H

#include <QString>

#include <QList>
#include <cctype>

#include "klinkstatus_export.h"

class QString;

typedef unsigned int uint;


/**
   Similar to std::string::find but return the index of the first char
   of the next word it finds.
   Case insensitive.
   e.g.
   findWord("Biltre larvado", "biltre") => 6
*/
int KLINKSTATUS_EXPORT findWord(QString const& s, QString const& palavra, int a_partir_do_indice = 0);

/**
   Similar to std::string::find but return the next index of the first char
   it finds.
   Case insensitive.
*/
int KLINKSTATUS_EXPORT findChar(QString const& s, QChar letra, int a_partir_do_indice = 0);

/**
   Same as findWord but non space chars are eliminated.
   e.g. 
   findWord("<a href=""></a>", "<a") => 2
   findSeparableWord("<a href=""></a>", "<a") => 2
 
   findWord("<\na href=""></a>", "<a") => -1
   findSeparableWord("<\na href=""></a>", "<a") => 3
*/
int KLINKSTATUS_EXPORT findSeparableWord(QString const& s, QString const& palavra, int a_partir_do_indice = 0);

/**
   Space means Unicode characters with decimal values 
   9 (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR), and 32 (Space).
*/
bool KLINKSTATUS_EXPORT isSpace(QChar c);

/**
 Return -1 if unsuccessful.
*/
int KLINKSTATUS_EXPORT nextNonSpaceChar(QString const& s, int i);
int KLINKSTATUS_EXPORT nextNonSpaceCharReverse(QString const& s, int i);
int KLINKSTATUS_EXPORT nextSpaceChar(QString const& s, int i);

int KLINKSTATUS_EXPORT nextCharDifferentThan(QChar c, QString const& s, int i);

/** Return a vector with the words */
QList<QString> KLINKSTATUS_EXPORT tokenizeWordsSeparatedByDots(QString const& s);
QList<QString> KLINKSTATUS_EXPORT tokenizeWordsSeparatedBy(QString const& s, const QChar& criteria);

/**
   If char 'caractere' is the last in the string 's' it is removed
*/
void KLINKSTATUS_EXPORT removeLastCharIfExists(QString& s, QChar caractere);

/**
   Remove whitespaces from the end of the string
*/
void KLINKSTATUS_EXPORT trimmedFromTheEnd(QString& s);


/**
   Case insensitive comparisons
*/
bool KLINKSTATUS_EXPORT equal(QString const& s1, QString const& s2);
bool KLINKSTATUS_EXPORT notEqual(QString const& s1, QString const& s2);

bool KLINKSTATUS_EXPORT equal(QChar c1, QChar c2);
bool KLINKSTATUS_EXPORT notEqual(QChar c1, QChar c2);


//_________________________________________________________________________

inline bool isSpace(QChar c)
{
    return c.isSpace();
}

inline bool equal(QString const& s1, QString const& s2)
{
	if(s1 == s2)
		return true;
	else
		return s1.toLower() == s2.toLower();
}

inline bool notEqual(QString const& s1, QString const& s2)
{
    return !(equal(s1, s2));
}

inline bool equal(QChar c1, QChar c2)
{
    return c1.toLower() == c2.toLower();
}

inline bool notEqual(QChar c1, QChar c2)
{
    return !(equal(c1, c2));
}

inline void removeLastCharIfExists(QString& s, QChar caractere)
{
	int index = s.length() - 1;
	if(index >= 0 && s[index] == caractere)
		s.remove(index);
}


#endif
