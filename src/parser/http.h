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

#include <QHttpResponseHeader>
#include <QString>


class HttpResponseHeader: public QHttpResponseHeader
{
public:

  HttpResponseHeader();
  HttpResponseHeader(const QHttpResponseHeader& header);
  explicit HttpResponseHeader(QString const& str);
  explicit HttpResponseHeader(int code, const QString& text = QString(), int majorVer = 1, int minorVer = 1);
  virtual ~HttpResponseHeader();

  void parseLocation();
  QString const& location() const;
  QString charset() const;
  
  /**
   * Parses the charset from this kind of server response: 
   * Content-Type: text/html; charset=EUC-JP
   * Return an empty string in case it doesn't find nothing.
   */
  static QString charset(QString const& contentTypeHttpHeaderLine);

private:

  QString location_;
};


inline HttpResponseHeader::HttpResponseHeader()
  : QHttpResponseHeader()
{
}

inline HttpResponseHeader::HttpResponseHeader(const QHttpResponseHeader& header)
  : QHttpResponseHeader(header)
{
}

inline HttpResponseHeader::HttpResponseHeader(QString const& str)
  : QHttpResponseHeader()
{
    // Needed - do NOT remove
	parse(str);
}

inline HttpResponseHeader::HttpResponseHeader(int code, const QString& text, int majorVer, int minorVer)
  : QHttpResponseHeader(code, text, majorVer, minorVer)
{

}

inline HttpResponseHeader::~HttpResponseHeader()
{
}

inline QString const& HttpResponseHeader::location() const
{
  return location_;
}

#endif
