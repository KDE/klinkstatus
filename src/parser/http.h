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

#ifndef HTTP_H
#define HTTP_H

#include <q3http.h>
#include <qstring.h>


class HttpResponseHeader: public Q3HttpResponseHeader
{
public:

  HttpResponseHeader();
  HttpResponseHeader(const Q3HttpResponseHeader & header);
  HttpResponseHeader(QString const& str);
  virtual ~HttpResponseHeader();

  void parseLocation();
  QString const& location() const;

private:

  QString location_;
};


inline HttpResponseHeader::HttpResponseHeader()
  : Q3HttpResponseHeader()
{
}

inline HttpResponseHeader::HttpResponseHeader(const Q3HttpResponseHeader & /*header*/)
  : Q3HttpResponseHeader()
{
}

inline HttpResponseHeader::HttpResponseHeader(QString const& str)
  : Q3HttpResponseHeader()
{
	parse(str);
}

inline HttpResponseHeader::~HttpResponseHeader()
{
}

inline QString const& HttpResponseHeader::location() const
{
  return location_;
}

#endif
