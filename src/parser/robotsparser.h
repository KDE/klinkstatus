/***************************************************************************
 *   Copyright (C) 2008 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef ROBOTSPARSER_H
#define ROBOTSPARSER_H

#include <KUrl>

#include "klinkstatus_export.h"

typedef KUrl::List KUrlList;


class KLINKSTATUS_EXPORT RobotsParser
{
public:
    RobotsParser();
    ~RobotsParser();

    bool canFetch(KUrl const& url);

    void parseRobotFileUrl(KUrl const& robotUrl);
    void setUserAgent(QString const& userAgent) { m_userAgent = userAgent; }

private:
//     bool canFetch(KUrl const& url, QString const& agent) const;
//     static QStringList parseUserAgents(QString const& line);
//     static QString parseDisallowedPath(QString const& line);

private:
    KUrl m_robotFileUrl;
    QString m_userAgent;
    KUrlList m_blockedUrls;
};

class KLINKSTATUS_EXPORT RobotCommand
{
public:
    enum CommandType {
        UNKNOWN,
        COMMENT,
        USER_AGENT,
        DISALLOW,
        ALLOW,
        SITEMAP
    };

    RobotCommand();
    RobotCommand(QString const& commandLine);

    void parseCommandLine(QString const& commandLine);

    CommandType getCommandType() const { return m_commandType; }
    QString const& getAttributeName() const { return m_attributeName; }
    QString const& getAttributeValue() const { return m_attributeValue; }
    
private:
    CommandType m_commandType;
    QString m_attributeName;
    QString m_attributeValue;
};

#endif
