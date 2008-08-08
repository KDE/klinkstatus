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

#ifndef KLINKSTATUS_EXPORT_H
#define KLINKSTATUS_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KLINKSTATUS_EXPORT
# if defined(MAKE_KLINKSTATUSCOMMON_LIB)
   /* We are building this library */ 
#  define KLINKSTATUS_EXPORT KDE_EXPORT
# else
   /* We are using this library */ 
#  define KLINKSTATUS_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KLINKSTATUS_EXPORT_DEPRECATED
#  define KLINKSTATUS_EXPORT_DEPRECATED KDE_DEPRECATED KLINKSTATUS_EXPORT
# endif

#endif
