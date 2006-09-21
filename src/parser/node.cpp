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

#include "node.h"
#include "mstring.h"
#include "url.h"
#include "../utils/utils.h"


/*
  Node________________________________________________________________________
*/

QString Node::getAttribute(QString const& atributo)
{
    QString attribute_;
    int fim = - 1;
    bool tem_aspas_ou_plicas = false;

    int inicio = findWord(content_, atributo);
    if(inicio != -1)
    {
        if(content_[inicio] == '"')
        {
            fim = content_.find("\"", inicio + 1);
            tem_aspas_ou_plicas = true;
        }
        else if(content_[inicio] == '\'')
        {
            fim = content_.find("'", inicio + 1);
            tem_aspas_ou_plicas = true;
        }
        else
        {
            int fim_bloco = nextSpaceChar(content_, inicio + 1);
            int fim_tag = content_.find(">", inicio + 1);
            int fim_aspas = content_.find("\"", inicio + 1);

            if(fim_bloco == -1 && fim_tag == -1 && fim_aspas == -1)
            {
                attribute_ = content_;
                malformed_ = true;
                return attribute_;
            }

            if(smallerUnsigned(fim_bloco, fim_tag) == -1 &&
                    smallerUnsigned(fim_bloco, fim_aspas) == -1)
                fim = fim_bloco;

            else if(smallerUnsigned(fim_tag, fim_aspas) == -1)
                fim = fim_tag;

            else
                fim = fim_aspas;
        }

        if(fim == -1)
        {
            attribute_ = content_;
            malformed_ = true;
            return attribute_;
        }

        attribute_ = content_.mid(inicio, fim-inicio);

        if(tem_aspas_ou_plicas)
        {
            attribute_ = attribute_.mid(1, attribute_.length() - 1);
        }
        else
        {
            ::stripWhiteSpace(attribute_);
        }
    }

    else
    {
        attribute_ = "";
    }
    ::decode(attribute_);

    return attribute_;
}


/*
  NodeLink________________________________________________________________________
*/

void NodeLink::parseAttributeHREF()
{
    if(findWord(content(), "HREF") == -1 &&
            findWord(content(), "NAME") == -1 &&
            findWord(content(), "TARGET") == -1)
    {
        kdDebug(23100) <<  "MALFORMED: " << endl
        << "NodeLink::parseAttributeHREF: " << content() << endl;
        setMalformed(true);
        return;
    }

    else if(findWord(content(), "HREF") != -1)
    {
        attribute_href_ = getAttribute("HREF=");

        if( !(malformed() || attribute_href_.isEmpty()) )
        {
            // Definnishr o tipo de link
            linktype_ = Url::resolveLinkType(attribute_href_);

            parseLinkLabel();
        }
    }
}

void NodeLink::parseLinkLabel()
{
    int fim_tag = 0;
    char proximo_caractere = ' ';
    
    do
    {
        fim_tag = content_.find(">", fim_tag);
 
        if(fim_tag != -1)
            proximo_caractere = QChar(content_[++fim_tag]);
 
    }
    while(fim_tag != -1 && proximo_caractere == '<'/*If the label starts by <*/);

    if(fim_tag != -1)
    {
        int fim_label = content_.find("<", fim_tag);

        if(fim_label != -1)
        {
            link_label_ =
                ::simplifyWhiteSpace(content_.mid(fim_tag,
                                                  fim_label - fim_tag));
        }
    }
}


/*
  NodeMETA________________________________________________________________________
*/

void NodeMETA::parseAttributeURL()
{
    if(attribute_http_equiv_.isEmpty())
        parseAttributeHTTP_EQUIV();

    if(upperCase(attribute_http_equiv_) == "REFRESH")
    {
        is_redirection_ = true;

        if(findWord(content(), "URL") == -1)
        {
            //setMalformed(true);
            return;
        }

        attribute_url_ = getAttribute("URL=");

        int aspas = -1;
        do
        {
            aspas = attribute_url_.find("\"");
            if(aspas != -1)
                attribute_url_.remove(aspas, 1);
        }
        while(aspas != -1);

        if(attribute_url_.isEmpty())
            kdDebug(23100) <<  "void NodeMeta::parseAttributeURL(): Assertion `!attribute_url_.isEmpty()' failed.\n"
            << content_ << endl << attribute_http_equiv_ << endl << attribute_url_ << endl;
        Q_ASSERT(!attribute_url_.isEmpty());

        linktype_ = Url::resolveLinkType(attribute_url_);
    }
}

QString NodeMETA::charset() const
{
    QString charset;
    QString content(atributoCONTENT());
    
    if(content.isEmpty())
        return charset;
    
    int index = content.find("charset=");
    if(index != -1) 
    {    
        index += QString("charset=").length();
        charset = content.mid(index, content.length() - index);
        charset = charset.stripWhiteSpace();
    }
    
//     kdDebug(23100) << "Charset: |" << charset << "|" << endl;
    return charset;
}

/*
  NodeIMG________________________________________________________________________
*/

void NodeIMG::parseAttributeSRC()
{
    if(findWord(content(), "SRC") == -1)
    {
        kdDebug(23100) <<  "MALFORMED_____________________________________________________________" << endl;
        kdDebug(23100) <<  "Conteudo: " << content() << endl;
        setMalformed(true);
        return;
    }

    attribute_src_ = getAttribute("SRC=");
    linktype_ = Url::resolveLinkType(attribute_src_);
}


/*
  NodeFRAME________________________________________________________________________
*/

void NodeFRAME::parseAttributeSRC()
{
    if(findWord(content(), "SRC") == -1)
    {
        //setMalformed(true);
        return;
    }

    attribute_src_ = getAttribute("SRC=");
    linktype_ = Url::resolveLinkType(attribute_src_);
}

