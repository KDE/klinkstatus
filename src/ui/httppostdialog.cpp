/*  This file is part of the KDE project
    Copyright (C) 2008 Paulo Moura Guedes <moura@kdewedev.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "httppostdialog.h"

#include <QHeaderView>


HttpPostDialog::HttpPostDialog(QWidget *parent)
  : KDialog(parent)
{
    setCaption(i18n("Login Input"));
    setButtons(KDialog::Ok);
    setDefaultButton(KDialog::Ok);
    showButtonSeparator(true);

    QWidget* widget = new QWidget(this);
    m_ui.setupUi(widget);

    m_ui.tableWidgetPostData->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    connect(m_ui.buttonAdd, SIGNAL(clicked()), this, SLOT(slotAddButtonClicked()));

    setMainWidget(widget);

    connect(this, SIGNAL(accepted()), this, SLOT(slotAccepted()));
}

HttpPostDialog::~HttpPostDialog()
{
}

void HttpPostDialog::setDomainField(QString const& domain)
{
    m_ui.lineeditDomain->setText(domain);
}

void HttpPostDialog::setPostUrlField(QString const& url)
{
    m_ui.lineeditPostUrl->setText(url);
}

void HttpPostDialog::slotAccepted()
{
    m_postUrl = "/login/login";
    m_postData = buildPostData();
}

QByteArray HttpPostDialog::buildPostData() const
{
    QByteArray data;
    QList<QByteArray> pairList;

    int currentRowCount = m_ui.tableWidgetPostData->rowCount();
    for(int i = 0; i != currentRowCount; ++i)
    {
        QString key = m_ui.tableWidgetPostData->item(i, 0)->text();
        if(key.isNull() || key.isEmpty()) {
            continue;
        }

        QString value = m_ui.tableWidgetPostData->item(i, 1)->text();

        pairList << (QUrl::toPercentEncoding(key) + "=" + QUrl::toPercentEncoding(value));
    }

    for(int i = 0; i != pairList.size(); ++i)
    {
        if(!data.isEmpty()) {
            data += QByteArray("&");
        }
        data += (pairList.at(i));
    }

    return data;
}

void HttpPostDialog::slotAddButtonClicked()
{
    QString key = m_ui.lineeditAddKey->text();
    QString value = m_ui.lineeditAddValue->text();

    QTableWidgetItem* keyItem = new QTableWidgetItem(key);
    QTableWidgetItem* valueItem = new QTableWidgetItem(value);

    int currentRowCount = m_ui.tableWidgetPostData->rowCount();
    m_ui.tableWidgetPostData->setRowCount(currentRowCount + 1);
    
    m_ui.tableWidgetPostData->setItem(currentRowCount, 0, keyItem);
    m_ui.tableWidgetPostData->setItem(currentRowCount, 1, valueItem);

    m_ui.lineeditAddKey->clear();
    m_ui.lineeditAddValue->clear();
}


#include "httppostdialog.moc"
