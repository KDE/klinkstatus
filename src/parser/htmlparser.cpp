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

#include "htmlparser.h"

#include <kapplication.h>
#include <kdebug.h>


HtmlParser::HtmlParser(QString const& documento)
        : document_(documento)
{
    assert(!documento.isEmpty());

    stripScriptContent();
    stripComments(); // after removing the script because comments in scripts have diferent sintaxe

    nodes_.reserve(estimativaLinks(documento.length() * 2)); // à confiança ;)

    parseNodesOfTypeA();
    parseNodesOfTypeLINK();
    parseNodesOfTypeMETA();
    parseNodesOfTypeIMG();
    parseNodesOfTypeFRAME();
    parseNodesOfTypeIFRAME();
    parseNodesOfTypeBASE();
}

bool HtmlParser::hasBaseUrl() const
{
    return (node_BASE.element() == Node::BASE &&
            !node_BASE.url().isEmpty());
}

NodeBASE const& HtmlParser::baseUrl() const
{
    assert(hasBaseUrl());
    return node_BASE;
}

vector<QString> const& HtmlParser::parseNodesOfType(QString const& tipo)
{
    QString node;
    QString doc = document_;
    int inicio = 0, fim = 0;

    aux_.clear();
    if(upperCase(tipo) == "A")
        aux_.reserve(estimativaLinks(document_.length() * 2));

    int i = 0;
    while(true)
    {
        ++i;
        inicio = findSeparableWord(doc, "<" + tipo);
        if(inicio == -1)
            return aux_;

        //if( (doc[inicio] != ' ' && doc[inicio] != '\n' && doc[inicio] != '\r') )
        if(!::isSpace(doc[inicio]))
        {
            doc.remove(0, QString("<" + tipo).length());
            continue;
        }

        if(upperCase(tipo) == "A")
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
        aux_.push_back(node);
        doc.remove(0, fim);

        if(i == 20)
        {
            kapp->processEvents();
            i = 0;
        }
    }

    return aux_;
}

int HtmlParser::endOfTag(QString const& s, int index, QChar end_of_tag)
{
    if( (uint)index >= s.length() )
        return -1;

    int _end_of_tag = s.find(end_of_tag, index);
    if(_end_of_tag == -1)
        return _end_of_tag;

    int open_aspas = s.find('"', index);
    if(open_aspas == -1)
        return _end_of_tag + 1;

    else if(_end_of_tag < open_aspas)
        return _end_of_tag + 1;

    else if( ((uint)open_aspas + 1) >= s.length() - 1 )
        return -1;

    else
    {
        int close_aspas = s.find('"', open_aspas + 1);
        if(close_aspas != -1)
            return endOfTag(s, close_aspas + 1, end_of_tag);
        else
        {
            kdDebug(2121) <<  "Mismatched quotes (\"): " << s.mid(index, _end_of_tag - index) << endl;
            //return -1;
            return _end_of_tag + 1;
        }
    }
}

vector<Node*> const& HtmlParser::nodes() const
{
    return nodes_;
}


void HtmlParser::parseNodesOfTypeA()
{
    vector<QString> const& aux = parseNodesOfType("A");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
    {
        nodes_.push_back( new NodeA(aux[i]) );
    }
}

void HtmlParser::parseNodesOfTypeLINK()
{
    vector<QString> const& aux = parseNodesOfType("LINK");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeLINK(aux[i]) );
}

void HtmlParser::parseNodesOfTypeMETA()
{
    vector<QString> const& aux = parseNodesOfType("META");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeMETA(aux[i]) );
}

void HtmlParser::parseNodesOfTypeIMG()
{
    vector<QString> const& aux = parseNodesOfType("IMG");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeIMG(aux[i]) );
}

void HtmlParser::parseNodesOfTypeFRAME()
{
    vector<QString> const& aux = parseNodesOfType("FRAME");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeFRAME(aux[i]) );
}

void HtmlParser::parseNodesOfTypeIFRAME()
{
    vector<QString> const& aux = parseNodesOfType("IFRAME");

    for(vector<QString>::size_type i = 0; i != aux.size(); ++i)
        nodes_.push_back( new NodeFRAME(aux[i]) );
}

void HtmlParser::parseNodesOfTypeBASE()
{
    QString node;
    QString doc = document_;
    int inicio = 0, fim = 0;

    inicio = findSeparableWord(doc, "<BASE");
    if(inicio == -1 || (doc[inicio] != ' ' && doc[inicio] != '\n') )
        return;

    fim = doc.find(">", inicio);
    if(fim == -1)
        return;

    node = doc.mid(inicio, fim-inicio);
    node_BASE.setNode(node);
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
                kdDebug(2121) <<  "End of comment is missing!" << endl;
                document_.remove(inicio - begin_comment_length, begin_comment_length);
            }
            else
            {
                comments_ += "\n" + document_.mid(inicio - begin_comment_length,
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
                kdDebug(2121) <<  "Malformed script tag!" << endl;
                document_.remove(inicio - begin_script_length, begin_script_length);
            }
            else
            {
                script_ += "\n" + document_.mid(inicio - begin_script_length,
                                                fim - inicio + begin_script_length);

                document_.remove(inicio - begin_script_length,
                                 fim - inicio + begin_script_length);
            }
        }
    }
    while(inicio != -1);
}




#include <iostream>
void HtmlParser::mostra() const
{
    kdDebug(2121) << "\nA:\n\n";
    for(unsigned int i = 0; i != nodes_.size(); ++i)
    {
        if(nodes_[i]->element() == Node::A)
            kdDebug(2121) << nodes_[i]->url() << "\t" << nodes_[i]->linkLabel() << endl;
    }
    kdDebug(2121) << "____________________________________________________________________" << endl;

    kdDebug(2121) << "\nLINK:\n\n";
    for(unsigned int i = 0; i != nodes_.size(); ++i)
    {
        if(nodes_[i]->element() == Node::LINK)
            kdDebug(2121) << nodes_[i]->url() << "\t" << nodes_[i]->linkLabel() << endl;
    }
    kdDebug(2121) << "____________________________________________________________________" << endl;

    kdDebug(2121) << "\nMETA:\n";
    for(unsigned int i = 0; i != nodes_.size(); ++i)
    {
        if(nodes_[i]->element() == Node::META)
        {
#if defined Q_WS_WIN
            NodeMETA* nm = (NodeMETA*)nodes_[i];
#else

            NodeMETA* nm = dynamic_cast<NodeMETA*>(nodes_[i]);
#endif

            kdDebug(2121) << nm->url() << endl
            << nm->atributoHTTP_EQUIV() << endl
            << nm->atributoNAME() << endl
            << nm->atributoCONTENT() << endl;
        }
    }
    kdDebug(2121) << "____________________________________________________________________" << endl;

    kdDebug(2121) << "\nIMG:\n\n";
    for(unsigned int i = 0; i != nodes_.size(); ++i)
    {
        if(nodes_[i]->element() == Node::IMG)
            kdDebug(2121) << nodes_[i]->url() << "\t"
            << nodes_[i]->linkLabel() << endl;
    }
    kdDebug(2121) << "____________________________________________________________________" << endl;

    kdDebug(2121) << "\nFRAME:\n\n";
    for(unsigned int i = 0; i != nodes_.size(); ++i)
    {
        if(nodes_[i]->element() == Node::FRAME)
            kdDebug(2121) << nodes_[i]->url() << endl;
    }
    kdDebug(2121) << "____________________________________________________________________" << endl;

    kdDebug(2121) << "\nBASE:\n\n";
    kdDebug(2121) << node_BASE.url() << endl;

    kdDebug(2121) << "____________________________________________________________________" << endl;

}

#ifdef HTMLPARSER

#include <fstream>

int main()
{
    //ifstream stream("aterraprometida.html");
    //ifstream stream("/var/www/html/STL/standard_library.html");
    //ifstream stream("/var/www/html/qt-doc/functions.html");
    ifstream stream("/var/www/html/index.html");

    QString content;
    while(stream)
    {
        char c;
        stream.get(c);
        content += c;
    }
    //  kdDebug(2121) << content << endl;
    kdDebug(2121) <<  "__________________________________________________________" << endl;
    HtmlParser parser(content);
    parser.mostra();
    kdDebug(2121) <<  "__________________________________________________________\n\n\n" << endl;
    vector<Node*> nods = parser.nodes();
    for(int i = 0; i != nods.size(); ++i)
    {
        if(nods[i]->element() == Node::META)
        {
            NodeMETA* nod_meta = (NodeMETA*)(nods[i]);
            //Node* nod_meta = nods[i];

            kdDebug(2121) << nod_meta->atributoCONTENT() << endl;
        }

    }
}


#endif
