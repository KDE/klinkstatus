/***************************************************************************
 *   Copyright (C) 2010 by Paulo Moura Guedes                              *
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

#include "robotsparser.h"

#include <KDebug>
#include <KIO/NetAccess>
#include <KIO/Job>

#include <QTextStream>


RobotsParser::RobotsParser()
{
}

RobotsParser::~RobotsParser()
{
}

void RobotsParser::parseRobotFileUrl(KUrl const& robotsUrl)
{
    KIO::Job* job = KIO::get(robotsUrl);
    QByteArray data;

    if(KIO::NetAccess::synchronousRun(job, 0, &data) )
    {
        QTextStream stream(data);

        QString line;
        bool applyToBot = false;
        do {
            line = stream.readLine();

            RobotCommand command(line);

            switch (command.getCommandType())
            {
                case RobotCommand::COMMENT: {
                    kDebug(23100) << "comment";
                    break;
                }
                case RobotCommand::USER_AGENT: {
                    QString userAgent = command.getAttributeValue();
                    kDebug(23100) << "user agent: " << userAgent;
                    
                    if(userAgent == "*" || userAgent == m_userAgent) {
                        kDebug(23100) << "applyToBot";
                        applyToBot = true;
                    }
                    else {
                        applyToBot = false;
                    }

                    break;
                }
                case RobotCommand::DISALLOW: {  // Disallow
                    kDebug(23100) << "disallow";
                    if (applyToBot)
                    {
                        QString url = command.getAttributeValue();
                        kDebug(23100) << "disallowed url: " << url;
                        if(!url.isEmpty())
                        {
                            m_blockedUrls << url;
                        }
                    }

                    break;
                }
                case RobotCommand::ALLOW: {  // Allow - only used by Google
                    break;
                }
                case RobotCommand::SITEMAP: { // Sitemap - points to Google sitemap
                    break;
                }
                default: {
                    break;
                }
            }
        } while (!line.isNull());
    }
}


bool RobotsParser::canFetch(KUrl const& url)
{
    if(m_blockedUrls.isEmpty()) {
        return true;
    }

    KUrl urlToCheck(url.path());
    if(urlToCheck.path() == "robots.txt") {
        return false;
    }

    for(int i = 0; i != m_blockedUrls.size(); ++i)
    {
        KUrl const& blockedUrl = m_blockedUrls.at(i);
        if(blockedUrl.isParentOf(urlToCheck)) {
            return false;
        }
    }

    return true;
}



// bool RobotsParser::canGet(KUrl const& url, QString const& agent) const
// {
//     return true;
// }
// 
// void RobotsParser::parse()
// {
//     KIO::Job* job = KIO::get(m_robotFileUrl);
//     QByteArray data;
// 
//     if(KIO::NetAccess::synchronousRun(job, 0, &data) )
//     {
//         QTextStream stream(data);
// 
//         QString line;
//         QStringList agents;
//         do {
//             line = stream.readLine();
// 
//             if(line.startsWith("User-agent:", Qt::CaseInsensitive)) {
//                 agents = parseUserAgents(line);
//             }
//             else if(line.startsWith("Disallow:", Qt::CaseInsensitive)) {
//                 QString path = parseDisallowedPath(line);
// 
//                 for(int i = 0; i != agents.size(); ++i) {
//                     m_AgentPathMap.insert(agents[i].toLower(), path);
//                 }
//             }
//             
//         } while (!line.isNull());
//     }
// }
// 
// QStringList RobotsParser::parseUserAgents(QString const& line)
// {
//     kDebug() << "line: " << line;
//   
//     QStringList agents;
//   
//     QString pattern = "(^User-agent:[\\s]*([\\w\\s])*)|(Disallow:\\s*(\\S*))";
//     QRegExp rx(pattern, Qt::CaseInsensitive);
// 
//     int pos = 0;
//     while((pos = rx.indexIn(line, pos)) != -1) {
// 
//         kDebug() << rx.cap(0);
//         kDebug() << rx.cap(1);
//         kDebug() << rx.cap(2);
//         kDebug() << rx.cap(3);
//         pos += rx.matchedLength();
//     }
// 
//     return agents;
// }
// 
// QString RobotsParser::parseDisallowedPath(QString const& line)
// {
//     kDebug() << "line: " << line;
//     
//     QString path;
// 
//     return path;
// }

// -------------------------------------------------

RobotCommand::RobotCommand()
    : m_commandType(RobotCommand::UNKNOWN)
{
}

RobotCommand::RobotCommand(QString const& commandLine)
    : m_commandType(UNKNOWN)
{
    parseCommandLine(commandLine);
}

void RobotCommand::parseCommandLine(QString const& _commandLine)
{
    QString commandLine = _commandLine.trimmed();
    if(commandLine.isEmpty()) {
        return;
    }

//     kDebug(23100) << "commandLine: " << commandLine;
    
    if(commandLine.startsWith("#")) {
        m_commandType = COMMENT;
        return;
    }

    int commentIndex = commandLine.indexOf("#");
    if(commentIndex != -1) {
        commandLine = commandLine.left(commentIndex);
    }

    if(commandLine.split(":", QString::SkipEmptyParts).size() < 2) {
        return;
    }

    m_attributeName = commandLine.section(":", 0, 0).trimmed().toLower();
//     kDebug(23100) << "attributeName: " << m_attributeName;
    m_attributeValue = commandLine.section(":", 1).trimmed();
//     kDebug(23100) << "attributeValue: " << m_attributeValue;
    
    if(m_attributeName == "user-agent") {
        m_commandType = USER_AGENT;
    }
    else if(m_attributeName == "disallow") {
        m_commandType = DISALLOW;
    }
    else if(m_attributeName == "allow") {
        m_commandType = ALLOW;
    }
    else if(m_attributeName == "sitemap") {
        m_commandType = SITEMAP;
    }
    else {
        m_commandType = UNKNOWN;
    }
}
