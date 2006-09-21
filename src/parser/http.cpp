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

#include "http.h"
#include "mstring.h"

#include <kdebug.h>

#include <iostream>



void HttpResponseHeader::parseLocation()
{
  QString cabecalho(toString());

  int location = findWord(cabecalho, "Location: ");  
  Q_ASSERT(location != -1);

  int fim_de_linha_1 = cabecalho.find('\n', location);
  int fim_de_linha_2 = cabecalho.find('\r', location);

  Q_ASSERT(fim_de_linha_1 != -1 || fim_de_linha_2 != -1);

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

QString HttpResponseHeader::charset() const
{
    return HttpResponseHeader::charset(value("content-type"));
}

QString HttpResponseHeader::charset(QString const& contentTypeHttpHeaderLine)
{
    QString _charset;
    
    if(contentTypeHttpHeaderLine.isEmpty())
        return _charset;
    
    int index = contentTypeHttpHeaderLine.find("charset=");
    if(index != -1)
        index += QString("charset=").length();
    else {
        index = contentTypeHttpHeaderLine.find("charset:");
        if(index != -1)
            index += QString("charset:").length();
    }
    
    if(index != -1) {
        _charset = contentTypeHttpHeaderLine.mid(index);
        _charset = _charset.stripWhiteSpace();
    }
    
//     kdDebug(23100) << "Charset: |" << _charset << "|" << endl;
    return _charset;

}
