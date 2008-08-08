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

#ifndef HTTPPOSTDIALOG_H
#define HTTPPOSTDIALOG_H

#include <KDialog>

#include "ui_httppostwidgetui.h"


class HttpPostDialog : public KDialog
{
Q_OBJECT
public:
    HttpPostDialog(QWidget* parent = 0);
    ~HttpPostDialog();

    QString const& postUrl() const { return m_postUrl; }
    QByteArray const& postData() const { return m_postData; }

    void setDomainField(QString const& domain);
    void setPostUrlField(QString const& url);

protected slots:
    void slotAccepted();

private slots:
    void slotAddButtonClicked();

private:
    QByteArray buildPostData() const;

private:
    Ui::HttpPostWidgetUi m_ui;
    QString m_postUrl;
    QByteArray m_postData;
};

#endif
