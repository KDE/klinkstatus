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

#ifndef SESSION_INTERFACE_H
#define SESSION_INTERFACE_H

#include <QWidget>

    
class WidgetInterface : public QWidget
{
public:
    WidgetInterface(QWidget* parent);
};

inline WidgetInterface::WidgetInterface(QWidget* parent)
    : QWidget(parent)
{
}
    
class PlayableWidgetInterface : public WidgetInterface
{
public:
    PlayableWidgetInterface(QWidget* parent);
    virtual ~PlayableWidgetInterface() = 0;
  
    bool inProgress() { return in_progress_; }
    bool stopped() { return stopped_; }
    bool paused() { return paused_; }

    virtual void slotStartSearch() = 0;
    virtual void slotPauseSearch() = 0;
    virtual void slotStopSearch() = 0;
    
protected:
    bool ready_;
    bool to_start_;
    bool to_pause_;
    bool to_stop_;
    bool in_progress_;
    bool paused_;
    bool stopped_;
};

inline PlayableWidgetInterface::PlayableWidgetInterface(QWidget* parent)
    : WidgetInterface(parent),
      ready_(true), to_start_(false), to_pause_(false), to_stop_(false),
      in_progress_(false), paused_(false), stopped_(true)
{
}

inline PlayableWidgetInterface::~PlayableWidgetInterface()
{
}

#endif

