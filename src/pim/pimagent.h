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

#ifndef PIMAGENT_H
#define PIMAGENT_H

#include <QString>

#include <klinkstatus_export.h>

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class KLINKSTATUS_EXPORT PimAgent
{
public:
    PimAgent();
    ~PimAgent();
    
    void setToEmail(QString const& email);
    void setSubject(QString const& subject);
    void setMessage(QString const& message);

    void sendMessage();
    
private:
    QByteArray compileMessage();
    
    /**
      * this will color each paragraph with a color depending on the
      * quotatation level.
      */
    static QString highlightText(const QString& text);

    /** this will color the text with a color depending on the quotation
      * level
      */
    static QString highlightParagraph( const QString& text);
    
private:
    QString m_transportName;
    
    QString m_name;
    QString m_fromEmail;
    
    QString m_toEmail;
    QString m_subject;
    QString m_message;
};

#endif
