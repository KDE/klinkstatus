/***************************************************************************
 *   Copyright (C) 2007 by Paulo Moura Guedes                              *
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

#include "klsfactory.h"

#include "klsconfig.h"
#include "ui/treeview.h"
#include "ui/sessionwidget.h"
    
#include <QStringList>
    

SessionWidget* KLSFactory::createSessionWidget(QWidget* parent)
{
    SessionWidget* session_widget = new SessionWidget(KLSConfig::maxConnectionsNumber(), 
            KLSConfig::timeOut(), parent);

    QStringList columns;
    
    columns.push_back(TreeView::URL_LABEL);
    columns.push_back(TreeView::STATUS_LABEL);
    if(KLSConfig::showMarkupStatus())
        columns.push_back(TreeView::MARKUP_LABEL);
    columns.push_back(TreeView::LINK_LABEL_LABEL);
    
    session_widget->setColumns(columns);

    // FIXME
//     session_widget->tree_view->restoreLayout(KLSConfig::self()->config(), "klinkstatus");

    return session_widget;
}

