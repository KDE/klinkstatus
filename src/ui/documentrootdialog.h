/***************************************************************************
 *   Copyright (C) 2006 by Paulo Moura Guedes                              *
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
#ifndef DOCUMENTROOTDIALOG_H
#define DOCUMENTROOTDIALOG_H

#include <kdialogbase.h>

class KURLRequester;

/**
	@author Paulo Moura Guedes <moura@kdewebdev.org>
*/
class DocumentRootDialog : public KDialogBase
{
Q_OBJECT
public:
    DocumentRootDialog(QWidget *parent, QString const& url);
    ~DocumentRootDialog();

    void setUrl(const QString& theValue) { m_url = theValue; }
    QString url() const { return m_url; }
    

protected:
    virtual void closeEvent (QCloseEvent*) {}
    
protected slots:
    virtual void reject() {}
    virtual void slotOk();
                
private slots:
    void slotTextChanged(const QString &);
    void slotReturnPressed(const QString &);

private:
    KURLRequester* m_urlRequester;
    QString m_url;
};

#endif
