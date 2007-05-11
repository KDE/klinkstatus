/***************************************************************************
 *   Copyright (C) 2006-2007 by Paulo Moura Guedes                              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef TIDYMARKUPVALIDATOR_H
#define TIDYMARKUPVALIDATOR_H

#include <KUrl>

#include <QObject>
#include <QString>
#include <QList>

#include "tidyx.h"

namespace Tidy
{

struct Message
{
    Message(ReportLevel _reportLevel,
            uint _line,
            uint _col,
            ctmbstr _message)
            : reportLevel(_reportLevel),
            line(_line),
            col(_col),
            message(_message)
    {}
            
    Message() 
    {}

    ReportLevel reportLevel;
    uint line;
    uint col;
    QString message;
};

typedef QList<Tidy::Message> TidyMessages;

class Doc : public Document
{
public:
    Doc();
    virtual ~Doc() {}

    void setErrorBuffer(Buffer& buffer);
    Buffer& getErrorBuffer() { return *errorBuffer_; }
    Buffer const& getErrorBuffer() const { return *errorBuffer_; }

    TidyMessages const& messages() const {return messages_; }

    // not lazy
    bool hasErrors() { return hasErrors_; }
    bool hasWarnings() { return hasWarnings_; }
    
protected:
    virtual Bool OnMessage(ReportLevel reportLevel, uint line, uint col, ctmbstr message);

protected:
    bool superficialValidation_;
    
private:
    Buffer* errorBuffer_;
    QList<Message> messages_;
    bool hasErrors_;
    bool hasWarnings_;
};


/**
    @author Paulo Moura Guedes <moura@kdewebdev.org>
    
*/
class MarkupValidator : public Doc
{
public:

    MarkupValidator(KUrl const& url, QString const& markup);
    virtual ~MarkupValidator();

    void validate(bool superficial = false);

    QString const& originalMarkup() const { return markup_; }
    QString const& validatedMarkup() const {return validated_markup_; }
    
    // lazy
    int numberOfErrors();
    int numberOfWarnings();
    // lazy
    TidyMessages errorMessages();
    TidyMessages warningMessages();
    
//     static bool hasErrors(TidyMessages const& messages);
//     static bool hasWarnings(TidyMessages const& messages);
//     
//     static int numberOfErrors(TidyMessages const& messages);
//     static int numberOfWarnings(TidyMessages const& messages);
//     
//     static TidyMessages errorMessages(TidyMessages const& all_messages);
//     static TidyMessages warningMessages(TidyMessages const& all_messages);

private:
    void setTidyOptions();
    static bool hasMessagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages);
    static int numberOfMessagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages);
    static TidyMessages messagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages);

private:
    KUrl const url_;
    QString const markup_;
    QString validated_markup_;
};

}

#endif
