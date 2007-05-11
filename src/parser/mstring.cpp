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

#include "mstring.h"


int findWord(QString const& s, QString const& palavra, int a_partir_do_indice)
{
    int index = s.indexOf(palavra, a_partir_do_indice, Qt::CaseInsensitive);

    if(index == -1)
        return index;
    else
        return (index + palavra.length());
}

int findChar(QString const& s, QChar letra, int a_partir_do_indice)
{
    int index = s.indexOf(letra, a_partir_do_indice, Qt::CaseInsensitive);
    if(index == -1)
        return index;
    else
        return index + 1;
}

/**
  The string palavra, must not have any spaces.
*/
int findSeparableWord(QString const& s_, QString const& palavra, int a_partir_do_indice)
{
    bool encontrou = true;
    QString s(s_);
    int indice_palavra = 0;
    int index = a_partir_do_indice;

    do
    {
        encontrou = true;
        indice_palavra = 0;

        index = findChar(s, (palavra[indice_palavra++]), index );

        if(index == -1)
        {
            return index;
        }
        --index;

        while(encontrou && indice_palavra != palavra.length() && index < s.length() && index >= 0)
        {
            index = nextNonSpaceChar(s, index);

            if(index == -1)
                return index;

            // Nao se incrementa o index porque isso jÃÂ¯ÃÂ¿ÃÂ½ÃÂ¯ÃÂ¿ÃÂ½feito com a funÃÂ¯ÃÂ¿ÃÂ½o nextNonSpaceChar
            encontrou = encontrou && !(notEqual(s[index], palavra[indice_palavra++]) );

        }
    }
    while(!encontrou && index < (int)s.length());

    if(encontrou && index < (int)s.length())
        return ++index;
    else
        return -1;
}

int nextNonSpaceChar(QString const& s, int i)
{
    ++i;
    //  while( (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')
    while(i < s.length() && isSpace(s[i]))
        ++i;

    if(i < s.length())
        return i;
    else
        return -1;
}


/**
   e.g.
   nextSpaceChar("o   biltre") => 1
*/
int nextSpaceChar(QString const& s, int i)
{
    //while( (s[i] != ' ' && s[i] != '\r' && s[i] != '\n' && s[i] != '\t') &&
    //i < s.size() )
    while(i < s.length() && !isSpace(s[i]))
        ++i;

    if(i < s.length())
        return i;
    else
        return -1;
}

int nextCharDifferentThan(QChar c, QString const& s, int i)
{
    while(i < s.length() && s[i] == c)
        ++i;

    if(i != s.length())
        return i;
    else
        return -1;
}

QList<QString> tokenizeWordsSeparatedByDots(QString const& _s)
{
    QList<QString> v;
    QString s(_s);

    while(true)
    {
        int inicio = 0;
        if(s[0] == '.')
            inicio = nextCharDifferentThan(QChar('.'), s, 0);
        if(inicio == -1)
            return v;

        int fim = s.indexOf('.', inicio);
        if(fim == -1)
        {
            v.push_back(s.mid(inicio));
            return v;
        }
        else
        {
            QString palavra = s.mid(inicio, fim - inicio);
            v.push_back(palavra);
            s.remove(0, fim);
        }
    }	
}

QList<QString> tokenizeWordsSeparatedBy(QString const& _s, const QChar & criteria)
{
    QList<QString> v;
    QString s(_s);

    while(true)
    {
        int inicio = 0;
        if(s[0] == criteria)
            inicio = nextCharDifferentThan(criteria, s, 0);
        if(inicio == -1)
            return v;

        int fim = s.indexOf(criteria, inicio);
        if(fim == -1)
        {
            v.push_back(s.mid(inicio));
            return v;
        }
        else
        {
            QString palavra = s.mid(inicio, fim - inicio);
            v.push_back(palavra);
            s.remove(0, fim);
        }
    }	
}
