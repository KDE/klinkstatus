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

#include <iostream>


using namespace std;


int findWord(QString const& s, QString const& palavra, uint a_partir_do_indice)
{
    int indice = s.find(palavra, a_partir_do_indice, false);

    if(indice == -1)
        return indice;
    else
        return (indice + palavra.length());
}

int findChar(QString const& s, QChar letra, uint a_partir_do_indice)
{
    int index = s.find(letra, a_partir_do_indice, false);
    if(index == -1)
        return index;
    else
        return index + 1;
}

/**
  The string palavra, must not have any spaces.
*/
int findSeparableWord(QString const& s_, QString const& palavra, uint a_partir_do_indice)
{
    bool encontrou = true;
    QString s(s_);
    uint indice_palavra = 0;
    int indice = a_partir_do_indice;

    do
    {
        encontrou = true;
        indice_palavra = 0;

        indice = findChar(s, (palavra[indice_palavra++]), indice );

        if(indice == -1)
        {
            return indice;
        }
        --indice;

        while(encontrou && indice_palavra != palavra.length() && indice < (int)s.length())
        {
            indice = nextNonSpaceChar(s, indice);

            if(indice == -1)
                return indice;

            // Nao se incrementa o indice porque isso j��feito com a fun�o nextNonSpaceChar
            encontrou = encontrou && !(notEqual(s[indice], palavra[indice_palavra++]) );

        }
    }
    while(!encontrou && indice < (int)s.length());

    if(encontrou && indice < (int)s.length())
        return ++indice;
    else
        return -1;
}

int nextNonSpaceChar(QString const& s, uint i)
{
    ++i;
    //  while( (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')
    while(isSpace(s[i])
            && i < s.length() )
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
int nextSpaceChar(QString const& s, uint i)
{
    //while( (s[i] != ' ' && s[i] != '\r' && s[i] != '\n' && s[i] != '\t') &&
    //i < s.size() )
    while(!isSpace(s[i]) &&
            i < s.length() )
        ++i;

    if(i < s.length())
        return i;
    else
        return -1;
}

int nextCharDifferentThan(QChar c, QString const& s, uint i)
{
    while(i < s.length() && s[i] == c)
        ++i;

    if(i != s.length())
        return i;
    else
        return -1;
}

vector<QString> tokenize(QString s)
{
    Q_ASSERT(!s.isEmpty());
    vector<QString> v;

    while(true)
    {
        int inicio = 0;
        //if(s[0] == ' ' || s[0] == '\t' || s[0] == '\r' || s[0] == '\n')
        if(isSpace(s[0]))
            inicio = nextNonSpaceChar(s, 0);
        if(inicio == -1)
            return v;

        int fim = nextSpaceChar(s, inicio);
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

vector<QString> tokenizeWordsSeparatedByDots(QString s)
{
    vector<QString> v;

    while(true)
    {
        int inicio = 0;
        if(s[0] == '.')
            inicio = nextCharDifferentThan(QChar('.'), s, 0);
        if(inicio == -1)
            return v;

        int fim = s.find('.', inicio);
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

vector<QString> tokenizeWordsSeparatedBy(QString s, QChar criteria)
{
    vector<QString> v;

    while(true)
    {
        int inicio = 0;
        if(s[0] == criteria)
            inicio = nextCharDifferentThan(criteria, s, 0);
        if(inicio == -1)
            return v;

        int fim = s.find(criteria, inicio);
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



#ifdef STRING
//c++ -g -o teste_string mstring.cpp -DSTRING
#include <fstream>

int main(int argc, char* argv[])
{
    string s;
    s = "S";
    s = "Afazer";
    s = "O MeU S    sdadsadd              ";
    s = "www.trolltech.com/search/qt-interest/bla bla%20Bla";
    s = "...http://w.ww..go.o.gle.p.t.......";

    /*
      ifstream stream("testeparser.html");
      string content;
      while(stream) {
        char c;
        stream.get(c);
        content += c;
      }
    */
    //  kdDebug(23100) << simplifyWhiteSpace(content) << endl;
    kdDebug(23100) << simplifyWhiteSpace(s) << endl;

    /*
      vector<string> v(tokenize(s));
      for(int i = 0; i != v.size(); ++i)
        kdDebug(23100) << v[i] << endl;
    */

    /*
      int i = nextSpaceChar(s, 0);
      i = nextNonSpaceChar(s, i);
      kdDebug(23100) << s.substr(i) << endl;
    */


    vector<string> v(tokenizeWordsSeparatedByDots(s));
    for(int i = 0; i != v.size(); ++i)
        kdDebug(23100) << v[i] << endl;

    removeLastCharIfExists(s, '/');
    kdDebug(23100) << s << endl;

    /*
      kdDebug(23100) << findChar(s, 'T') << endl;
      kdDebug(23100) << findWord(s, "trolltech") << endl;
      kdDebug(23100) << findWord(s, "TROLLTECH") << endl;
      kdDebug(23100) << findWord(s, "TROLLTECH", 2) << endl;
    */
    /*
      stripWhiteSpace(s);
      kdDebug(23100) << s << endl;
    */
}


#endif
