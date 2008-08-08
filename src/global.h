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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include <QTimer>
#include <QDomDocument>
class QLabel;

namespace KParts {
    class ReadOnlyPart;
    class StatusBarExtension;
}
class KStatusBar;

#include "klinkstatus_export.h"

using namespace KParts;


/**
@author Paulo Moura Guedes
*/
class KLINKSTATUS_EXPORT Global
{
public:
    static Global* getInstance();

    Global();
    ~Global();
    void setKLinkStatusPart(ReadOnlyPart* part);
    void setPartDestroyed();

    KStatusBar* statusBar() const;
    void setStatusBarText(QString const& text, bool permanent = false);
    void addStatusBarPermanentItem(QWidget* widget);

    /**
     * The XML document where sessions setting (url, depth, etc) are saved
     **/
    QDomDocument& sessionsDocument();
    void saveSessionsDocument() const;
    void findCurrentSession(QString const& url, QDomElement& element);

private:
    class GlobalPrivate;
    GlobalPrivate* const global_private;
};


#endif
