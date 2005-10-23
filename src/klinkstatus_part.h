/***************************************************************************
 *   Copyright (C) 2004 by Paulo Moura Guedes                              *
 *   moura@kdewebdev.org                                                        *
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

#ifndef _KLINKSTATUSPART_H_
#define _KLINKSTATUSPART_H_

#include <kparts/part.h>

class TabWidgetSession;
class ActionManager;

class QWidget;
class QPainter;

class KURL;
class KAboutData;
class KAboutApplication;
class KAction;

class KLinkStatusPart: public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    KLinkStatusPart(QWidget *parentWidget, const char *widgetName,
                    QObject *parent, const char *name,
                    const QStringList& args);
    virtual ~KLinkStatusPart();

    /** Reimplemented to disable and enable Save action */
    virtual void setModified(bool modified);

    static KAboutData* createAboutData();

protected:
    /** This must be implemented by each part */
    virtual bool openFile();
    virtual bool openURL (const KURL &url);
    //	virtual bool saveFile(){};

protected slots:
    void slotNewLinkCheck();
    void slotOpenLink();
    void slotClose();
    void slotConfigureKLinkStatus();
/*    void slotDisplayAllLinks();
    void slotDisplayGoodLinks();
    void slotDisplayBadLinks();
    void slotDisplayMalformedLinks();
    void slotDisplayUndeterminedLinks();*/
    void slotAbout();
    void slotReportBug();
    
// private slots:
//     void slotEnableDisplayLinksActions();
//     void slotDisableDisplayLinksActions();

private:
    void initGUI();

private:
    static const char description_[];
    static const char version_[];

    ActionManager* action_manager_;

    TabWidgetSession* tabwidget_;
    KAboutApplication* m_dlgAbout;
};

#endif // _KLINKSTATUSPART_H_
