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
#include "documentrootdialog.h"

#include <kurlrequester.h>
#include <klocale.h>
#include <kurl.h>

#include <QString>
#include <QLayout>
#include <QLabel>
//Added by qt3to4:
#include <Q3VBoxLayout>


DocumentRootDialog::DocumentRootDialog(QWidget *parent, KUrl const& url)
        : KDialog(parent),
        m_url(url)
{
    setCaption( i18n( "Choose a Document Root" ) );
    setButtons( KDialog::Ok );
    setDefaultButton( KDialog::Ok );
    showButtonSeparator( true );
    QWidget* page = new QWidget(this);
    setMainWidget(page);
    Q3VBoxLayout* topLayout = new Q3VBoxLayout(page, 0, spacingHint());

    QLabel* label = new QLabel(i18n("As you are using a protocol different than HTTP, \nthere is no way to guess where the document root is, \nin order to resolve relative URLs like the ones started with \"/\".\n\nPlease specify one:"), page);
    topLayout->addWidget(label);

    m_urlRequester = new KUrlRequester(page);
    m_urlRequester->setUrl(url);
    m_urlRequester->setMinimumWidth(fontMetrics().maxWidth()*20);
    m_urlRequester->setFocus();
    topLayout->addWidget(m_urlRequester);

    topLayout->addStretch(10);

    //     setInitialSize(configDialogSize("klinkstatus"));

    m_urlRequester->setMode(KFile::Directory);
//     enableButtonOk(false);

    connect(m_urlRequester, SIGNAL(textChanged (const QString &)),
            this, SLOT(slotTextChanged (const QString &)));
    connect(m_urlRequester, SIGNAL(returnPressed (const QString &)),
            this, SLOT(slotReturnPressed (const QString &)));
    connect(m_urlRequester, SIGNAL(urlSelected (const KUrl &)),
            this, SLOT(slotUrlChanged( const KUrl &)));
}

DocumentRootDialog::~DocumentRootDialog()
{
#ifdef _GNUC
	#warning "kde4: port it"
#endif
    //saveDialogSize("klinkstatus", true);
}

void DocumentRootDialog::slotReturnPressed( const QString & )
{
    slotOk();
}

void DocumentRootDialog::slotUrlChanged( const KUrl & s)
{
    enableButtonOk(s.isValid());
}

void DocumentRootDialog::slotTextChanged( const QString & s )
{
    KUrl url(s);
    enableButtonOk(!s.isEmpty() && url.isValid());
}

void DocumentRootDialog::slotOk( )
{
    m_url = m_urlRequester->url();

    KDialog::accept();
}



#include "documentrootdialog.moc"
