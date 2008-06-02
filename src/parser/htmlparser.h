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

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <QString>

#include <QList>


#include "mstring.h"
#include "node.h"

typedef unsigned int uint;



class HtmlParser
{
public:

  HtmlParser();
  HtmlParser(QString const& documento);
  ~HtmlParser();

  QList<Node*> const& nodes() const;
  QList<Node*> const& anchorNodes() const;
  bool hasBaseUrl() const;
  bool hasTitle() const;
  bool hasContentType() const;
  NodeBASE const& baseUrl() const;
  NodeTITLE const& title() const;
  NodeMETA const& contentTypeMetaNode() const;

  /**
   * Convenience function for performance as it only parse in order 
   * to get the charset.
   */
  static QString findCharsetInMetaElement(QString const& html);

  // test:
  void mostra() const;

private:

  QList<QString> const& parseNodesOfType(QString const& element);
  /**
   * Vector nodes passed for performance.
   */
  static void parseNodesOfType(QString const& element, QString const& doc, QList<QString>& nodes);

  void parseNodesOfTypeA();
  void parseNodesOfTypeAREA();
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

  QList<QString> aux_; // for what the hell is this? looks ugly... maybe I was drunk, can't remember
  QList<Node*> nodes_;
  QList<Node*> anchor_nodes_;
  NodeBASE node_BASE_;
  NodeTITLE node_TITLE_;
  NodeMETA node_META_content_type_;
  bool is_content_type_set_;

  QString document_;
  QString script_; // Fica aqui guardado (JavaScript, etc)
  QString comments_;
};


inline bool HtmlParser::hasContentType() const
{
    return is_content_type_set_;
}

#endif
