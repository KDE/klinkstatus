/***************************************************************************
 *   Copyright (C) 2004-2007 by Paulo Moura Guedes                              *
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

#ifndef KLSHISTORYCOMBO_H
#define KLSHISTORYCOMBO_H

#include <khistorycombobox.h>
class QKeyEvent;
class KConfig;

/**
@author Paulo Moura Guedes
Based on KonqCombo
*/
class KLSHistoryCombo : public KHistoryComboBox
{
    Q_OBJECT

public:
    explicit KLSHistoryCombo(QWidget* parent);
    ~KLSHistoryCombo();

    void addCurrentItem(QString const& text);
    
    void init();
    void loadItems();
    void saveItems();

protected:
    virtual bool eventFilter(QObject* o, QEvent* ev);
    void selectWord(QKeyEvent* e);
    
private:
    static bool items_saved_;
};

#endif
