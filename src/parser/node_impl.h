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

#ifndef NODE_IMPL_H
#define NODE_IMPL_H

inline Node::Node()
        : is_redirection_(false), malformed_(false)
{}

inline Node::~Node()
{
    //kDebug(23100) <<  "/";
}

inline Node::Node(const QString & content)
        : content_(content), is_redirection_(false), malformed_(false)
{}

inline void Node::setNode(const QString & content)
{
    content_ = content;
    parse();
}

inline QString Node::content() const
{
    return content_;
}

inline bool Node::malformed() const
{
    return malformed_;
}

inline void Node::setMalformed(bool flag)
{
    malformed_ = flag;
}

inline Node::LinkType Node::linkType() const
{
    return linktype_;
}

inline Node::Element Node::element() const
{
    return element_;
}

inline void Node::setLinkType(Node::LinkType const& lt)
{
    linktype_ = lt;
}

inline bool Node::isRedirection() const
{
    return is_redirection_;
}

// class NodeLink_______________________________________________________

inline NodeLink::NodeLink()
        : Node()
{}

inline NodeLink::NodeLink(const QString & content)
        : Node(content)
{
    parse();
}

inline void NodeLink::parse()
{
    parseAttributeHREF();
}

inline QString NodeLink::url() const
{
    return attribute_href_;
}

inline QString NodeLink::linkLabel() const
{
    return link_label_;
}

inline QString NodeLink::mailto() const
{
    Q_ASSERT(linktype_ == Node::mailto);

    QString href = KCharsets::resolveEntities(attribute_href_);

    int inicio = findWord(href, "MAILTO:");
    Q_ASSERT(inicio != -1);

    return href.mid(inicio);
}

inline bool NodeLink::isLink() const
{
    if(Node::linkType() != Node::mailto && !url().isEmpty())
        return true;
    else
        return false;
}

// class NodeA_______________________________________________________

inline NodeA::NodeA(const QString & content)
        : NodeLink(content)
{
    element_ = A;
    parse();
}

inline QString NodeA::attributeNAME() const
{
    return attribute_name_;
}

inline void NodeA::parse()
{
    parseAttributeNAME();
}

inline void NodeA::parseAttributeNAME()
{
    attribute_name_ = getAttribute("NAME=");
    //kDebug(23100) << "NodeA::parseAttributeNAME: " << attribute_name_;
}

// class NodeAREA_______________________________________________________
        
inline NodeAREA::NodeAREA(const QString & content)
    : NodeLink(content)
{
    element_ = AREA;
    parse();
}
            
inline QString NodeAREA::attributeTITLE() const
{
    return attribute_title_;
}
                
inline void NodeAREA::parse()
{
    parseAttributeTITLE();
}
                    
inline void NodeAREA::parseAttributeTITLE()
{
    attribute_title_ = getAttribute("TITLE=");
//kDebug(23100) << "NodeAREA::parseAttributeTITLE: " << attribute_title_;
}

// class NodeLINK________________________________________

inline NodeLINK::NodeLINK(const QString & content)
        : NodeLink(content)
{
    element_ = LINK;
}

// class NodeMeta________________________________________

inline NodeMETA::NodeMETA()
    : Node()
{
    element_ = META;
}

inline NodeMETA::NodeMETA(QString const& content)
        : Node(content)
{
    element_ = Node::META;
    parse();
}

inline QString NodeMETA::url() const
{
    return attribute_url_;
}

inline QString NodeMETA::linkLabel() const
{
    return link_label_;
}

inline bool NodeMETA::isLink() const
{
  if(attribute_http_equiv_.toUpper() == "REFRESH" &&
            findWord(content(), "URL") != -1)
    {
        // Q_ASSERT(findWord(content(), "URL") != -1); // not necessarily
        return true;
    }
    else
        return false;
}

inline QString NodeMETA::atributoHTTP_EQUIV() const
{
    return attribute_http_equiv_;
}

inline QString NodeMETA::atributoNAME() const
{
    return attribute_name_;
}

inline QString NodeMETA::atributoCONTENT() const
{
    return attribute_content_;
}

inline bool NodeMETA::isRedirection() const
{
    return
        attribute_http_equiv_.toUpper() == "REFRESH";
}

inline void NodeMETA::parse()
{
    parseAttributeHTTP_EQUIV();
    parseAttributeNAME();
    parseAttributeCONTENT();

    parseAttributeURL();
}

inline void NodeMETA::parseAttributeHTTP_EQUIV()
{
    attribute_http_equiv_ = getAttribute("HTTP-EQUIV=");
}

inline void NodeMETA::parseAttributeNAME()
{
    attribute_name_ = getAttribute("NAME=");
}

inline void NodeMETA::parseAttributeCONTENT()
{
    attribute_content_ = getAttribute("CONTENT=");
//     kDebug(23100) << "CONTENT: " << attribute_content_;
}


// class NodeIMG________________________________________

inline NodeIMG::NodeIMG(const QString & content)
        : Node(content)
{
    element_ = IMG;
    parse();
}

inline void NodeIMG::parse()
{
    parseAttributeSRC();
    parseAttributeTITLE();
    parseAttributeALT();
}

inline QString NodeIMG::url() const
{
    return attribute_src_;
}

inline QString NodeIMG::linkLabel() const
{
    if(!attribute_title_.isEmpty())
        return attribute_title_;
    else
        return attribute_alt_;
}

inline bool NodeIMG::isLink() const
{
    if(!url().isEmpty())
        return true;
    else
        return false;
}

inline void NodeIMG::parseAttributeTITLE()
{
    attribute_title_ = getAttribute("TITLE=");
}

inline void NodeIMG::parseAttributeALT()
{
    attribute_alt_ = getAttribute("ALT=");
}


// class NodeFRAME________________________________________

inline NodeFRAME::NodeFRAME(const QString & content)
        : Node(content)
{
    element_ = FRAME;
    parse();
}

inline void NodeFRAME::parse()
{
    parseAttributeSRC();
}

inline QString NodeFRAME::url() const
{
    return attribute_src_;
}

inline QString NodeFRAME::linkLabel() const
{
    return link_label_;
}

inline bool NodeFRAME::isLink() const
{
    if(!url().isEmpty())
        return true;
    else
        return false;
}

// class NodeBASE________________________________________

inline NodeBASE::NodeBASE()
        : NodeLink()
{
    element_ = BASE;
}

inline NodeBASE::NodeBASE(const QString & content)
        : NodeLink(content)
{
    element_ = BASE;
}

inline bool NodeBASE::isLink() const
{
    return false;
}

// class NodeTITLE________________________________________

inline NodeTITLE::NodeTITLE()
        : Node()
{
    element_ = TITLE;
    parse();
}

inline NodeTITLE::NodeTITLE(const QString & content)
        : Node(content)
{
    element_ = TITLE;
    parse();
}

inline QString NodeTITLE::url() const
{
    return QString();
}

inline QString NodeTITLE::linkLabel() const
{
    return QString();
}

inline void NodeTITLE::parse()
{
    parseAttributeTITLE();
}

inline bool NodeTITLE::isLink() const
{
    return false;   
}

inline QString NodeTITLE::attributeTITLE() const
{
    return attribute_title_;   
}

inline void NodeTITLE::parseAttributeTITLE()
{
    attribute_title_ = content_;
    attribute_title_.replace("<TITLE>", "", Qt::CaseInsensitive);
    attribute_title_.replace("</TITLE>", "", Qt::CaseInsensitive);
    attribute_title_.trimmed();
    
    //kDebug(23100) << "TITLE: " << attribute_title_;
}

#endif // NODE_IMPL_H
