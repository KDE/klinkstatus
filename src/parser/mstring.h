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

class QString;

typedef unsigned int uint;


/* Similar to std::string::find but return the next index of the last char
   of the first word it finds.
   Case insensitive.
   e.g.
   findWord("Biltre larvado", "biltre") => 6
*/
int findWord(QString const& s, QString const& palavra, int a_partir_do_indice = 0);

/**
   Similar to std::string::find but return the next index of the first char
   it finds.
   Case insensitive.
*/
int findChar(QString const& s, QChar letra, int a_partir_do_indice = 0);

/**
   Same as findWord but non space chars are eliminated.
   e.g. 
   findWord("<a href=""></a>", "<a") => 2
   findSeparableWord("<a href=""></a>", "<a") => 2
 
   findWord("<\na href=""></a>", "<a") => -1
   findSeparableWord("<\na href=""></a>", "<a") => 3
*/
int findSeparableWord(QString const& s, QString const& palavra, int a_partir_do_indice = 0);

/**
   Space means Unicode characters with decimal values 
   9 (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR), and 32 (Space).
*/
bool isSpace(QChar c);

/**
 Return -1 if unsuccessful.
*/
int nextNonSpaceChar(QString const& s, int i);
int nextNonSpaceCharReverse(QString const& s, int i);
int nextSpaceChar(QString const& s, int i);

int nextCharDifferentThan(QChar c, QString const& s, int i);

/** Return a vector with the words */
QList<QString> tokenizeWordsSeparatedByDots(QString const& s);
QList<QString> tokenizeWordsSeparatedBy(QString const& s, const QChar& criteria);

/**
   If char 'caractere' is the last in the string 's' it is removed
*/
void removeLastCharIfExists(QString& s, QChar caractere);

/**
   Remove whitespaces from the end of the string
*/
void trimmedFromTheEnd(QString& s);


/**
   Case insensitive comparisons
*/
bool equal(QString const& s1, QString const& s2);
bool notEqual(QString const& s1, QString const& s2);

bool equal(QChar c1, QChar c2);
bool notEqual(QChar c1, QChar c2);


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
