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

#include "markupvalidator.h"

#include <kdebug.h>

namespace Tidy
{

Doc::Doc()
    : Document(), hasErrors_(false), hasWarnings_(false)
{
    Create();
}

void Doc::setErrorBuffer(Buffer & buffer)
{
    errorBuffer_ = &buffer;
    tidySetErrorBuffer(_tdoc, errorBuffer_);
}

Bool Doc::OnMessage(ReportLevel reportLevel, uint line, uint col, ctmbstr message)
{
//     kDebug(23100) << "Message: " << message;
    
    if(!superficialValidation_)
        messages_.push_back(Message(reportLevel, line, col, message));
    
    if(!hasErrors_ && reportLevel == TidyError)
        hasErrors_ = true;
    if(!hasWarnings_ && reportLevel == TidyWarning)
        hasWarnings_ = true;
    
    return yes;
}



MarkupValidator::MarkupValidator(KUrl const& url, QString const& markup)
    : Doc(), url_(url), markup_(markup)
{}

MarkupValidator::~MarkupValidator()
{}

void MarkupValidator::validate(bool superficial)
{
    superficialValidation_ = superficial;

    // TODO
//     setTidyOptions();

    Buffer errorBuffer;
    setErrorBuffer(errorBuffer);
        
    // FIXME encoding!
    ParseString(markup_.toAscii());
    CleanAndRepair();
    RunDiagnostics();
    
    Buffer output;
    SaveBuffer(output);
    
    validated_markup_ = QString((ctmbstr)output.Data());
}

// bool MarkupValidator::hasErrors()
// {
//     return MarkupValidator::hasMessagesOfType(TidyError, messages());
// }
// 
// bool MarkupValidator::hasWarnings()
// {
//     return MarkupValidator::hasMessagesOfType(TidyWarning, messages());
// }

int MarkupValidator::numberOfErrors()
{
    return MarkupValidator::numberOfMessagesOfType(TidyError, messages());
}

int MarkupValidator::numberOfWarnings()
{
    return MarkupValidator::numberOfMessagesOfType(TidyWarning, messages());
}

TidyMessages MarkupValidator::errorMessages()
{
    return messagesOfType(TidyError, messages());
}

TidyMessages MarkupValidator::warningMessages()
{
    return messagesOfType(TidyWarning, messages());
}


// bool MarkupValidator::hasErrors(TidyMessages const & messages)
// {
//     return MarkupValidator::hasMessagesOfType(TidyError, messages);
// }
// 
// bool MarkupValidator::hasWarnings(TidyMessages const & messages)
// {
//     return MarkupValidator::hasMessagesOfType(TidyWarning, messages);
// }
// 
// int MarkupValidator::numberOfErrors(TidyMessages const& messages)
// {
//     return MarkupValidator::numberOfMessagesOfType(TidyError, messages);
// }
// 
// int MarkupValidator::numberOfWarnings(TidyMessages const& messages)
// {
//     return MarkupValidator::numberOfMessagesOfType(TidyWarning, messages);
// }
// 
// TidyMessages MarkupValidator::errorMessages(TidyMessages const& all_messages)
// {
//     return messagesOfType(TidyError, all_messages);
// }
// 
// TidyMessages MarkupValidator::warningMessages(TidyMessages const& all_messages)
// {
//     return messagesOfType(TidyWarning, all_messages);
// }


bool MarkupValidator::hasMessagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages)
{
    for (TidyMessages::const_iterator it = messages.begin(); it != messages.end(); ++it)
    {
        if((*it).reportLevel == reportLevel)
            return true;
    }
    return false;
}

int MarkupValidator::numberOfMessagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages)
{
    int count = 0;
    
    for (TidyMessages::const_iterator it = messages.begin(); it != messages.end(); ++it)
    {
        if((*it).reportLevel == reportLevel)
            count += 1;
    }
    return count;
}

TidyMessages MarkupValidator::messagesOfType(ReportLevel const& reportLevel, TidyMessages const& messages)
{
    TidyMessages filtered_messages;
    
    for (TidyMessages::const_iterator it = messages.begin(); it != messages.end(); ++it)
    {
        if((*it).reportLevel == reportLevel)
            filtered_messages.push_back(*it);
    }
    return filtered_messages;
}

}

