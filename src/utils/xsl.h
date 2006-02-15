/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
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
#ifndef XSL_H
#define XSL_H

#include <qobject.h>

class XSLTPrivate;

/**
    @author Paulo Moura Guedes <moura@kdewebdev.org>
    
    Taken from kopetexsl. Kudos to the Kopete team.
 * 
 * This class provides an easy to use interface to basic
 * libxslt transformations.
*/
class XSLT : public QObject
{
    Q_OBJECT

    Q_PROPERTY( Flags flags READ flags WRITE setFlags )
    Q_PROPERTY( bool isValid READ isValid )

    Q_SETS( Flags )

public:
    /**
     * Special flags to be used during the transformation process. Passed
     * into the engine as processing instructions.
     */
    enum Flags
    {
        TransformAllMessages = 1
    };

    /**
     * Constructor.
     *
     * Constructs a new XSLT parser using the provided XSLT document
     */
    XSLT( const QString &xsltDocument, QObject *parent = 0L );

    virtual ~XSLT();

    /**
     * Set the XSLT document
     *
     * @return an ORed set of @ref Flags, or 0 if none
     */
    void setXSLT( const QString &document );

    /**
     * Transforms the XML string using the XSLT document, synchronously
     *
     * @param xmlString The source XML
     * @return The result of the transformation
     */
    QString transform( const QString &xmlString );

    /**
     * Transforms the XML string using the XSLT document, asynchronously
     *
     * @param xmlString The source XML
     * @param target The QObject that contains the slot to be executed when processing is complete
     * @param slotCompleted A slot that accepts a QVariant & paramater, that is the result
     * of the transformation
     */
    void transformAsync( const QString &xmlString, QObject *target, const char *slotCompleted );

    /**
     * Check whether the XSLT document is valid
     *
     * @return Whether the document represents a valid XSLT stylesheet
     */
    bool isValid() const;

    /**
     * @return An ORed list of Flags that the current stylesheet provides via processing instructions
     */
    unsigned int flags() const;

    /**
     * Sets flags to be used for the transformation.
     *
     * @param flags An ORed list of flags
     */
    void setFlags( unsigned int flags );

private:
    XSLTPrivate *d;
};

#endif
