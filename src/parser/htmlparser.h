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

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <qstring.h>

#include <vector>


#include "mstring.h"
#include "node.h"

#include <iostream>

using namespace std;

typedef unsigned int uint;



class HtmlParser
{
public:

  HtmlParser();
  HtmlParser(QString const& documento);
  ~HtmlParser();

  vector<Node*> const& nodes() const;
  bool hasBaseUrl() const;
  bool hasTitle() const;
  NodeBASE const& baseUrl() const;
  NodeTITLE const& title() const;
  static uint estimativaLinks(uint doc_size);

  // test:
  void mostra() const;

private:

  vector<QString> const& parseNodesOfType(QString const& tipo);

  void parseNodesOfTypeA();
  void parseNodesOfTypeLINK();
  void parseNodesOfTypeMETA();
  void parseNodesOfTypeIMG();
  void parseNodesOfTypeFRAME();
  void parseNodesOfTypeIFRAME();
  void parseNodesOfTypeBASE();
  void parseNodesOfTypeTITLE();

  void stripComments();
  void stripScriptContent();

  /**
     Return the index of the next character of the end of tag.
     e.g.
     endOfTag("<img src=\"bad > luck\">") => 22 (not 15)
  */
  static int endOfTag(QString const& s, int index = 0, QChar end_of_tag = '>');

private:

  vector<QString> aux_;
  vector<Node*> nodes_;
  NodeBASE node_BASE_;
  NodeTITLE node_TITLE_;

  QString document_;
  QString script_; // Fica aqui guardado (JavaScript, etc)
  QString comments_;
};


inline HtmlParser::~HtmlParser()
{
  //kdDebug(23100) <<  "*";
}

inline uint HtmlParser::estimativaLinks(uint doc_size)
{
  return doc_size / 100; // valor estimado...
}

#endif
