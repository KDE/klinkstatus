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

#include "pimagent.h"

#include <kmime/kmime_message.h>
#include <kpimutils/linklocator.h>
#include <mailtransport/transport.h>
#include <mailtransport/transportmanager.h>
#include <mailtransport/transportjob.h>
#include <mailtransport/transport.h>
#include <kpimidentities/identity.h>
#include <kpimidentities/identitymanager.h>

#include <KDebug>
#include <KGlobal>
#include <KComponentData>
#include <KAboutData>

#include <QTextCodec>

#include "klsconfig.h"


PimAgent::PimAgent()
{
    if(KLSConfig::useSystemIdentity()) {
        kDebug() << "useSystemIdentity";
        
        KPIMIdentities::IdentityManager identityManager(false, 0, "IdentityManager");

        KPIMIdentities::Identity const& identity = identityManager.defaultIdentity();
        m_name = identity.fullName();
        m_fromEmail = identity.emailAddr();
    }
    else {
        kDebug() << "do not useSystemIdentity";
        
        m_name = KLSConfig::userName();
        m_fromEmail = KLSConfig::userEmail();
        
        kDebug() << "name: " << m_name;
        kDebug() << "fromEmail: " << m_fromEmail;
    }

    m_transportName = MailTransport::TransportManager::self()->defaultTransportName();
//     kDebug(23100) << m_transportName;
}

PimAgent::~PimAgent()
{
}

void PimAgent::setToEmail(QString const& to)
{
    m_toEmail = to;
}

void PimAgent::setSubject(QString const& subject)
{
    m_subject = subject;
}

void PimAgent::setMessage(QString const& message)
{
    m_message = message;
}

void PimAgent::sendMessage()
{
    kDebug(23100) << "PimAgent::sendMessage";
    
    if(m_name.isEmpty() || m_fromEmail.isEmpty()) {
        kWarning() << "UseSystemIdentity is true but settings are not complete! Using system defaults...";
        
        m_name = KLSConfig::userName();
        m_fromEmail = KLSConfig::userEmail();
    }
    
    QByteArray const messageData = compileMessage();

    if(messageData.isEmpty()) {
        kWarning(23100) << "E-Mail message data is empty, aborting";
        return;
    }

    MailTransport::TransportJob* job =
            MailTransport::TransportManager::self()->createTransportJob(m_transportName);
    
    if(!job) {
        kError(23100) << "Not possible to create SMTP Job!";
        return;
    }

    job->setData(messageData);
    job->setSender(m_fromEmail);
    
    QStringList toAddresses(m_toEmail);
    job->setTo(toAddresses);

    MailTransport::TransportManager::self()->schedule(job);
}
    
QByteArray PimAgent::compileMessage()
{
    kDebug(23100) << "PimAgent::compileMessage";
    
    using KMime::Message;
    using KMime::Content;

    if(m_name.isEmpty() || m_fromEmail.isEmpty()) {
        kError(23100) << "PIM settings are not defined, aborting";
        return QByteArray();
    }

    Message* newMessage = new Message();
    newMessage->contentType()->setMimeType("multipart/mixed");
    newMessage->contentType()->setBoundary(KMime::multiPartBoundary());

    // From
    KMime::Headers::From* fromAddress = new KMime::Headers::From();
    fromAddress->addAddress(m_fromEmail.trimmed().toLatin1(), m_name.toLatin1());
    newMessage->setHeader(fromAddress);

    // To
    KMime::Headers::To* toAddress = new KMime::Headers::To(newMessage);    
    KMime::Types::Mailbox mailbox;
    mailbox.setAddress(m_toEmail.trimmed().toLatin1());
    toAddress->addAddress(mailbox);
    newMessage->setHeader(toAddress);

    // Subject
    KMime::Headers::Subject* Subject = new KMime::Headers::Subject;
    Subject->fromUnicodeString(m_subject, "utf-8");
    newMessage->setHeader(Subject);

    // Date
    KMime::Headers::Date* date = new KMime::Headers::Date;
    date->setDateTime(KDateTime::currentLocalDateTime());
    newMessage->setHeader(date);

    // User Agent
    KMime::Headers::UserAgent* userAgent= new KMime::Headers::UserAgent;
    userAgent->fromUnicodeString( "KLinkStatus-v" +
            KGlobal::mainComponent().aboutData()->version(), "utf-8");
    newMessage->setHeader(userAgent);

    // add a newMessageid.
    KMime::Headers::MessageID* mi = new KMime::Headers::MessageID;
    mi->generate(m_fromEmail.mid(m_fromEmail.indexOf("@") + 1).toLatin1());
    newMessage->setHeader(mi);

    // Make the html-part of the same message.
    Content* c_html = new Content();
    c_html->setDefaultCharset("utf-8");
    c_html->contentType()->from7BitString("text/html");
    c_html->contentType()->setCharset("utf-8");

    {
        using KPIMUtils::LinkLocator;
//         const int flags = LinkLocator::LinkLocator::PreserveSpaces |
//                 LinkLocator::LinkLocator::HighlightText;
                
//             c_html->fromUnicodeString(highlightText(
//                                       LinkLocator::LinkLocator::convertToHtml(m_message, flags)));
//         c_html->fromUnicodeString(highlightText(m_message));        
        
        c_html->fromUnicodeString(m_message);
    }

    c_html->assemble();
    
    newMessage->addContent(c_html);
    newMessage->assemble();

    const QByteArray result = newMessage->encodedContent(true) + "\r\n";
//     kDebug(23100) << "newMessage compiled: " << endl << result;
    
    return result;
}

QString PimAgent::highlightText(const QString& text)
{
    // make the quotation colors.
    const QStringList temp = text.split("\n");
    QString result;
    QStringList::const_iterator it = temp.begin();
    while(it != temp.end())
    {
        result.append(highlightParagraph((*it)+'\n'));
        ++it;
    }
    return result;
}

QString PimAgent::highlightParagraph( const QString& text)
{
    //kDebug(50002) << "received: " << text << endl;

    bool found = false;
    QColor color;
    if (text.startsWith("&gt;&gt;&gt;&gt;")
        || text.startsWith("&gt; &gt; &gt; &gt;")
        || text.startsWith("||||") )
    {
        found = true;
        color = Qt::darkGreen;
    }
    else if (text.startsWith("&gt;&gt;&gt;") || text.startsWith("&gt; &gt; &gt;")
             || text.startsWith("|||") )
    {
        found = true;
        color = Qt::darkRed;
    }
    else if (text.startsWith("&gt;&gt;") || text.startsWith("&gt; &gt;")
             || text.startsWith("||") )
    {
        found = true;
        color = Qt::blue;
    }
    else if (text.startsWith("&gt;") || text.startsWith('|') )
    {
        found = true;
        color = Qt::red;
    }

    if (found)
        return "<font color=\""+color.name()+"\">"+text+"</font>";
    else
        return text;
}


