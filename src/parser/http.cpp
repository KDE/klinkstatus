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
#include "mstring.h"

#include <cassert>
#include <iostream>



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

