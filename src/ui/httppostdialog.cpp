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

#include "httppostdialog.h"

#include <KUrl>

#include <QHeaderView>
#include <QDomElement>

#include "global.h"
#include "parser/url.h"
#include "ui/sessionwidget.h"


HttpPostDialog::HttpPostDialog(QString const& searchUrl, QWidget *parent)
  : KDialog(parent), m_searchUrl(searchUrl)
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

    init();
}

HttpPostDialog::~HttpPostDialog()
{
}

void HttpPostDialog::init()
{
    QDomElement element;
    Global::getInstance()->findCurrentSession(m_searchUrl, element);

    KUrl url = Url::normalizeUrl(m_searchUrl);
    setDomainField(url.host());
    
    if(!element.isNull()) {
        QDomNode node = element.namedItem("login");
        if(!node.isNull() && node.isElement()) {
            QDomElement loginElement = node.toElement();
                
            SessionTO sessionTO;
            sessionTO.load(element);

            loadSessionTO(sessionTO);

            return;
        }
    }
    
    setPostUrlField(url.path());
}

void HttpPostDialog::loadSessionTO(SessionTO const& session)
{
    setPostUrlField(session.postUrl);

    QByteArray postData = session.postData;
    kDebug(23100) << postData;

    QList<QByteArray> tokens = postData.split('&');
    kDebug(23100) << tokens.size();
    for(int i = 0; i != tokens.size(); ++i)
    {
        QByteArray token = tokens[i];
        kDebug(23100) << token;
        QList<QByteArray> pair = token.split('=');
        kDebug(23100) << pair;

        QString key = QUrl::fromPercentEncoding(pair[0]);
        QString value = QUrl::fromPercentEncoding(pair[1]);

        QTableWidgetItem* keyItem = new QTableWidgetItem(key);
        QTableWidgetItem* valueItem = new QTableWidgetItem(value);

        int currentRowCount = m_ui.tableWidgetPostData->rowCount();
        m_ui.tableWidgetPostData->setRowCount(currentRowCount + 1);
        
        m_ui.tableWidgetPostData->setItem(currentRowCount, 0, keyItem);
        m_ui.tableWidgetPostData->setItem(currentRowCount, 1, valueItem);
    }
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
    m_postUrl = m_ui.lineeditPostUrl->text();
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
