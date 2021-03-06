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


#include "klshistorycombo.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kcompletionbox.h>
#include <kdebug.h>
#include <kstandardshortcut.h>
#include <kglobalsettings.h>
#include <QKeyEvent>
#include <QEvent>
#include <QLineEdit>
#include "klsconfig.h"


bool KLSHistoryCombo::items_saved_ = false;


KLSHistoryCombo::KLSHistoryCombo(QWidget *parent)
        : KHistoryComboBox(true, parent)
{
    setMaxCount(KLSConfig::maxCountComboUrl());
    
    setDuplicatesEnabled(false);
    
    connect(this, SIGNAL(activated(const QString&)),
            this, SLOT(addToHistory(const QString&)));
}

KLSHistoryCombo::~KLSHistoryCombo()
{}

void KLSHistoryCombo::init()
{
    loadItems();
}

void KLSHistoryCombo::addCurrentItem(QString const& text)
{
    int previous_count = count();
    addToHistory(text);
    if(previous_count != count())
        setCurrentIndex(0);
}

void KLSHistoryCombo::saveItems()
{
    if(items_saved_)
        return;

    QStringList items = historyItems();

    KLSConfig::setComboUrlHistory(items);
    KLSConfig::self()->writeConfig();
    
    items_saved_ = true;
}

void KLSHistoryCombo::loadItems()
{
    clear();
    
    QStringList items = KLSConfig::comboUrlHistory();

    bool block = signalsBlocked();
    blockSignals( true );
    
    setHistoryItems(items, true);

    blockSignals(block);

//     setCompletionMode(KGlobalSettings::completionMode());
}

bool KLSHistoryCombo::eventFilter( QObject *o, QEvent *ev )
{
    // Handle Ctrl+Del/Backspace etc better than the Qt widget, which always
    // jumps to the next whitespace.
    QLineEdit *edit = lineEdit();
    if ( o == edit ) {
        int type = ev->type();
        if ( type == QEvent::KeyPress ) {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );

            if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) {
//                 m_modifier = e->state();
                return false;
            }

            // ### TODO this is hackish; better create a kaction and connect to its slot
            const QList<QKeySequence> deleteWordBack = KStandardShortcut::deleteWordBack();
            const QList<QKeySequence> deleteWordForward = KStandardShortcut::deleteWordForward();
            const QKeySequence thisKey(e->key() | e->modifiers());

            if ( deleteWordBack.contains(thisKey) ||
                 deleteWordForward.contains(thisKey) ||
                 ((e->modifiers() & Qt::ControlModifier) &&
                   (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right) ) ) {
                selectWord(e);
                e->accept();
                return true;
            }
        }

        else if ( type == QEvent::MouseButtonDblClick ) {
            edit->selectAll();
            return true;
        }
    }
    return KComboBox::eventFilter( o, ev );
}

/*
   Handle Ctrl+Cursor etc better than the Qt widget, which always
   jumps to the next whitespace. This code additionally jumps to
   the next [/#?:], which makes more sense for URLs. The list of
   chars that will stop the cursor are '/', '.', '?', '#', ':'.
*/
void KLSHistoryCombo::selectWord(QKeyEvent *e)
{
    QLineEdit* edit = lineEdit();
    QString text = edit->text();
    int pos = edit->cursorPosition();
    int pos_old = pos;
    int count = 0;

    // TODO: make these a parameter when in kdelibs/kdeui...
    QList<QChar> chars;
    chars << QChar('/') << QChar('.') << QChar('?') << QChar('#') << QChar(':');
    bool allow_space_break = true;

    if( e->key() == Qt::Key_Left || e->key() == Qt::Key_Backspace ) {
        do {
            pos--;
            count++;
            if( allow_space_break && text[pos].isSpace() && count > 1 )
                break;
        } while( pos >= 0 && (chars.indexOf(text[pos]) == -1 || count <= 1) );

        if( e->modifiers() & Qt::ShiftModifier ) {
                  edit->cursorForward(true, 1-count);
        }
        else if(  e->key() == Qt::Key_Backspace ) {
            edit->cursorForward(false, 1-count);
            QString text = edit->text();
            int pos_to_right = edit->text().length() - pos_old;
            QString cut = text.left(edit->cursorPosition()) + text.right(pos_to_right);
            edit->setText(cut);
            edit->setCursorPosition(pos_old-count+1);
        }
        else {
            edit->cursorForward(false, 1-count);
        }
     }
     else if( e->key() == Qt::Key_Right || e->key() == Qt::Key_Delete ){
        do {
            pos++;
            count++;
                  if( allow_space_break && text[pos].isSpace() )
                      break;
        } while( pos < (int) text.length() && chars.indexOf(text[pos]) == -1 );

        if( e->modifiers() & Qt::ShiftModifier ) {
            edit->cursorForward(true, count+1);
        }
        else if(  e->key() == Qt::Key_Delete ) {
            edit->cursorForward(false, -count-1);
            QString text = edit->text();
            int pos_to_right = text.length() - pos - 1;
            QString cut = text.left(pos_old) +
               (pos_to_right > 0 ? text.right(pos_to_right) : QString() );
            edit->setText(cut);
            edit->setCursorPosition(pos_old);
        }
        else {
            edit->cursorForward(false, count+1);
        }
    }
}


#include "klshistorycombo.moc"
