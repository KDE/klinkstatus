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

#ifndef NODULO_H
#define NODULO_H

#include "mstring.h"

#include <QString>

#include <kdebug.h>

using namespace std;

typedef unsigned int uint;


class Node
{
public:

    enum Element {
        A,
        AREA,
        LINK,
        META,
        IMG,
        FRAME,
        BASE,
        TITLE
    };
    enum LinkType {
        href,
        file_href,
        mailto,
        relative
    };

    Node();
    Node(const QString & content);
    virtual ~Node();

    QString getAttribute(const QString & atributo);
    virtual QString url() const = 0;
    virtual QString linkLabel() const = 0; // URL label
    virtual void setNode(const QString & content);
    virtual void parse() = 0;
    void setMalformed(bool flag = true);
    virtual void setLinkType(LinkType const& lt);

    QString content() const;
    bool malformed() const;
    LinkType linkType() const;
    Element element() const;
    virtual bool isLink() const = 0;

    bool isRedirection() const;

protected:

    Element element_;
    LinkType linktype_;
    QString link_label_;
    QString content_;
    bool is_redirection_;
    bool malformed_;
};


class NodeLink: public Node
{
public:
    NodeLink();
    NodeLink(const QString & content);
    ~NodeLink()
    {}
    ;

    virtual void parse();

    virtual QString url() const;
    virtual QString linkLabel() const; // URL label
    virtual QString mailto() const;
    virtual bool isLink() const;

private:
    virtual void parseAttributeHREF();
    void parseLinkLabel();

private:
    QString attribute_href_;
};

class NodeA: public NodeLink
{
public:
    NodeA(const QString & content);
    ~NodeA()
    {}
    ;
    QString attributeNAME() const;

    virtual void parse();

private:
    void parseAttributeNAME();

private:
    QString attribute_name_;
};

class NodeAREA: public NodeLink
{
public:
    NodeAREA(const QString & content);
    ~NodeAREA() {};
    
    QString attributeTITLE() const;

    virtual void parse();

private:
    void parseAttributeTITLE();

private:
    QString attribute_title_;
};


class NodeLINK: public NodeLink
{
public:
    NodeLINK(const QString & content);
    ~NodeLINK()
    {}
    ;
};

class NodeMETA: public Node
{
public:
    NodeMETA(const QString & content);
    ~NodeMETA()
    {}
    ;

    virtual QString url() const;
    virtual QString linkLabel() const;
    virtual bool isLink() const;
    QString atributoHTTP_EQUIV() const;
    QString atributoNAME() const;
    QString atributoCONTENT() const;
    bool isRedirection() const;

    virtual void parse();

private:
    /**
       Procura se existem os atributos HTTP-EQUIV=Refresh e URL=...
       Se existir considera o content do atributo URL como um link.
       ex: <META HTTP-EQUIV=Refresh CONTENT="10; URL=http://www.htmlhelp.com/">
    */
    void parseAttributeURL();

    void parseAttributeHTTP_EQUIV();
    void parseAttributeNAME();
    void parseAttributeCONTENT();

private:
    QString attribute_http_equiv_;
    QString attribute_url_;
    QString attribute_name_;
    QString attribute_content_;
};

class NodeIMG: public Node
{
public:
    NodeIMG(const QString & content);
    ~NodeIMG()
    {}
    ;

    virtual void parse();

    virtual QString url() const;
    virtual QString linkLabel() const; // Image label
    virtual bool isLink() const;

private:
    void parseAttributeSRC();
    void parseAttributeTITLE();
    void parseAttributeALT();

private:
    QString attribute_src_;
    QString attribute_title_;
    QString attribute_alt_;
};

class NodeFRAME: public Node
{
public:
    NodeFRAME(const QString & content);
    ~NodeFRAME()
    {}
    ;

    virtual void parse();
    virtual QString url() const;
    virtual QString linkLabel() const;
    virtual bool isLink() const;

private:
    void parseAttributeSRC();

private:
    QString attribute_src_;
};

class NodeBASE: public NodeLink
{
public:
    NodeBASE();
    NodeBASE(const QString & content);
    ~NodeBASE()
    {}
    ;

    virtual bool isLink() const;
};

class NodeTITLE: public Node
{
public:
    NodeTITLE();
    NodeTITLE(const QString & content);
    ~NodeTITLE()
    {}
    ;

    virtual QString url() const;
    virtual QString linkLabel() const;
    virtual void parse();
    virtual bool isLink() const;
    
    QString attributeTITLE() const;

private:
    void parseAttributeTITLE();

private:
    QString attribute_title_;
};


#include "node_impl.h"

#endif
