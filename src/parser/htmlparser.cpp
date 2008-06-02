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

#include "htmlparser.h"

#include <kapplication.h>
#include <kdebug.h>


HtmlParser::HtmlParser(QString const& documento)
    : is_content_type_set_(false), document_(documento)
{
//     Q_ASSERT(!documento.isEmpty());
    if(documento.isEmpty())
        return;
  
    stripScriptContent();
    stripComments(); // after removing the script because comments in scripts have diferent sintax

    parseNodesOfTypeA();
    parseNodesOfTypeAREA();
    parseNodesOfTypeLINK();
    parseNodesOfTypeMETA();
    parseNodesOfTypeIMG();
    parseNodesOfTypeFRAME();
    parseNodesOfTypeIFRAME();
    parseNodesOfTypeBASE();
    parseNodesOfTypeTITLE();
}

HtmlParser::~HtmlParser()
{
}

bool HtmlParser::hasBaseUrl() const
{
    return (node_BASE_.element() == Node::BASE &&
            !node_BASE_.url().isEmpty());
}

NodeBASE const& HtmlParser::baseUrl() const
{
    Q_ASSERT(hasBaseUrl());
    return node_BASE_;
}

NodeMETA const& HtmlParser::contentTypeMetaNode() const
{
    Q_ASSERT(hasContentType());
    return node_META_content_type_;
}

bool HtmlParser::hasTitle() const
{
    return (node_TITLE_.element() == Node::TITLE &&
            !node_TITLE_.attributeTITLE().isEmpty());
}

NodeTITLE const& HtmlParser::title() const
{
    Q_ASSERT(hasTitle());
    return node_TITLE_;
}

QList<QString> const& HtmlParser::parseNodesOfType(QString const& element)
{
    HtmlParser::parseNodesOfType(element, document_, aux_);
    return aux_;
}

void HtmlParser::parseNodesOfType(QString const& tipo, QString const& document, QList<QString>& nodes)
{
    QString node;
    QString doc(document);
    int inicio = 0, fim = 0;

    nodes.clear();

    while(true)
    {
        inicio = findSeparableWord(doc, '<' + tipo);
        if(inicio == -1)
            return;

        //if( (doc[inicio] != ' ' && doc[inicio] != '\n' && doc[inicio] != '\r') )
        if(!::isSpace(doc[inicio]))
        {
            doc.remove(0, QString('<' + tipo).length());
            continue;
        }

        if(tipo.toUpper() == "A")
            fim = findWord(doc, "</A>", inicio);
        else
        {
            //fim = findChar(doc, '>', inicio + 1);
            fim = endOfTag(doc, inicio, '>');
        }

        if(fim == -1)
        {
            doc.remove(0, 1);
            continue;
        }

        int tag_begining_go_back = (tipo.length() + QString("<").length());
        node = doc.mid(inicio - tag_begining_go_back,
                       fim - inicio + tag_begining_go_back);
        nodes.push_back(node);
//         kDebug(23100) << "NODE: " << node << endl;
        doc.remove(0, fim);
    }
}

int HtmlParser::endOfTag(QString const& s, int index, QChar end_of_tag)
{
    if(index >= s.length())
        return -1;

    int _end_of_tag = s.indexOf(end_of_tag, index);
    if(_end_of_tag == -1)
        return _end_of_tag;

    int open_aspas = s.indexOf('"', index);
    if(open_aspas == -1)
        return _end_of_tag + 1;

    else if(_end_of_tag < open_aspas)
        return _end_of_tag + 1;

    else if((open_aspas + 1) >= s.length() - 1)
        return -1;

    else
    {
        int close_aspas = s.indexOf('"', open_aspas + 1);
        if(close_aspas != -1)
            return endOfTag(s, close_aspas + 1, end_of_tag);
        else
        {
            kDebug(23100) <<  "Mismatched quotes (\"): " << s.mid(index, _end_of_tag - index);
            //return -1;
            return _end_of_tag + 1;
        }
    }
}

QList<Node*> const& HtmlParser::nodes() const
{
    return nodes_;
}

QList<Node*> const& HtmlParser::anchorNodes() const
{
    return anchor_nodes_;
}


void HtmlParser::parseNodesOfTypeA()
{
    QList<QString> const& aux = parseNodesOfType("A");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
    {
        NodeA* node = new NodeA(aux[i]);
        nodes_.push_back(node);

        if(!node->attributeNAME().isEmpty()) {
            anchor_nodes_.push_back(new NodeA(aux[i]));
        }
    }
}

void HtmlParser::parseNodesOfTypeAREA()
{
    QList<QString> const& aux = parseNodesOfType("AREA");
                
    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
    {
        nodes_.push_back( new NodeAREA(aux[i]) );
    }
}

void HtmlParser::parseNodesOfTypeLINK()
{
    QList<QString> const& aux = parseNodesOfType("LINK");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeLINK(aux[i]) );
}

void HtmlParser::parseNodesOfTypeMETA()
{
    QList<QString> const& aux = parseNodesOfType("META");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
    {
        NodeMETA* node = new NodeMETA(aux[i]);
        nodes_.push_back(node);
        
        if(!is_content_type_set_ && node->atributoHTTP_EQUIV().toLower() == QString("Content-Type").toLower()) {
            is_content_type_set_ = true;
            node_META_content_type_.setNode(aux[i]);
        }
    }
}

QString HtmlParser::findCharsetInMetaElement(QString const& html)
{
    QList<QString> metaTags;
    parseNodesOfType("META", html, metaTags);
    
    for(QList<QString>::size_type i = 0; i != metaTags.size(); ++i)
    {
        NodeMETA node(metaTags[i]);
        
        if(node.atributoHTTP_EQUIV().toLower() == QString("Content-Type").toLower()) {
            return node.charset();
        }
    }
    return QString();
}

void HtmlParser::parseNodesOfTypeIMG()
{
    QList<QString> const& aux = parseNodesOfType("IMG");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeIMG(aux[i]) );
}

void HtmlParser::parseNodesOfTypeFRAME()
{
    QList<QString> const& aux = parseNodesOfType("FRAME");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeFRAME(aux[i]) );
}

void HtmlParser::parseNodesOfTypeIFRAME()
{
    QList<QString> const& aux = parseNodesOfType("IFRAME");

    for(QList<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeFRAME(aux[i]) );
}

void HtmlParser::parseNodesOfTypeBASE()
{
    QString node;
    QString doc = document_;
    int inicio = 0, fim = 0;

    inicio = findSeparableWord(doc, "<BASE");
    if(inicio == -1 || !doc[inicio].isSpace())
        return;

    fim = doc.indexOf('>', inicio);
    if(fim == -1)
        return;

    node = doc.mid(inicio, fim-inicio);
    node_BASE_.setNode(node);
}

void HtmlParser::parseNodesOfTypeTITLE()
{
    QString node;
    QString doc = document_;
    int inicio = 0, fim = 0;

    inicio = findSeparableWord(doc, "<TITLE>");
    if(inicio == -1)
        return;

    fim = findSeparableWord(doc, "</TITLE>", inicio);
    if(fim == -1)
        return;
    
    node = doc.mid(inicio, fim-inicio);

    node_TITLE_.setNode(node);
}


void HtmlParser::stripComments()
{
    QString begin_comment = "<!--";
    QString end_comment = "-->";
    uint const begin_comment_length = begin_comment.length();

    int inicio = -1;
    do
    {
        inicio = findWord(document_, begin_comment);
        if(inicio != -1)
        {
            int fim = findWord(document_, end_comment, inicio);
            if(fim == -1)
            {
                kDebug(23100) <<  "End of comment is missing!";
                document_.remove(inicio - begin_comment_length, begin_comment_length);
            }
            else
            {
                comments_ += '\n' + document_.mid(inicio - begin_comment_length,
                                                  fim - inicio + begin_comment_length);
                document_.remove(inicio - begin_comment_length, fim - inicio + begin_comment_length);
            }
        }
    }
    while(inicio != -1);
}

void HtmlParser::stripScriptContent()
{
    int inicio = -1;
    QString const begin_script = "<script";
    QString const end_script = "</script>";
    uint const begin_script_length = begin_script.length();

    do
    {
        inicio = findWord(document_, begin_script);
        if(inicio != -1)
        {
            int fim = findWord(document_, end_script, inicio);

            if(fim == -1)
            {
                kDebug(23100) <<  "Malformed script tag!";
                document_.remove(inicio - begin_script_length, begin_script_length);
            }
            else
            {
                script_ += '\n' + document_.mid(inicio - begin_script_length,
                                                fim - inicio + begin_script_length);

                document_.remove(inicio - begin_script_length,
                                 fim - inicio + begin_script_length);
            }
        }
    }
    while(inicio != -1);
}
