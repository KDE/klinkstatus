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

#include "http.h"

#include <cassert>
#include <iostream>

#include "mstring.h"

using namespace std;


void HttpResponseHeader::parseLocation()
{
  QString cabecalho(toString());

  int location = findWord(cabecalho, "Location: ");  
  assert(location != -1);

  int fim_de_linha_1 = cabecalho.find('\n', location);
  int fim_de_linha_2 = cabecalho.find('\r', location);

  assert(fim_de_linha_1 != -1 || fim_de_linha_2 != -1);

  int fim_de_linha;

  if(fim_de_linha_1 == -1 && fim_de_linha_2 != -1) 
    fim_de_linha = fim_de_linha_2;
  
  else if(fim_de_linha_2 == -1 && fim_de_linha_1 != -1) 
    fim_de_linha = fim_de_linha_1;
  
  else if(fim_de_linha_1 < fim_de_linha_2) 
    fim_de_linha = fim_de_linha_1;

  else fim_de_linha = fim_de_linha_2;

  location_ = cabecalho.mid(location, fim_de_linha - location);
}


#ifdef HTTPRESPONSEHEADER
// c++ -g -o testehttp http.C string.C -I/$QTDIR/include -L/$QTDIR/lib -lqt-mt -DHTTPRESPONSEHEADER

#include <string>

string parseLocation(string cabecalho)
{
  cerr << cabecalho << endl;

  int location = findWord(cabecalho, "Location: ");  
//  int location = cabecalho.find("Location: ");
  assert(location != -1);

  int fim_de_linha_1 = cabecalho.find("\n", location);
  int fim_de_linha_2 = cabecalho.find("\r", location);

  assert(fim_de_linha_1 != -1 || fim_de_linha_2 != -1);

  int fim_de_linha;
  
  if(fim_de_linha_1 == -1 && fim_de_linha_2 != -1) 
    fim_de_linha = fim_de_linha_2;
  
  else if(fim_de_linha_2 == -1 && fim_de_linha_1 != -1) 
    fim_de_linha = fim_de_linha_1;
  
  else if(fim_de_linha_1 < fim_de_linha_2) 
    fim_de_linha = fim_de_linha_1;

  else fim_de_linha = fim_de_linha_2;

  string location_ = cabecalho.substr(location, fim_de_linha - location);

  cerr << "Location: " << location_ << endl;  

  return location_;
}

#include <fstream>
int main()
{
  ifstream stream("parselocation.html");
  string content;
  while(stream) {
    char c;
    stream.get(c);
    content += c;
  }
//  cout << content << endl;

  parseLocation(content);
  
}


#endif
