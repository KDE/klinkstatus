//
// C++ Implementation: klshistorycombo
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "klshistorycombo.h"
#include "klsconfig.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kcompletionbox.h>
#include <kdebug.h>
#include <kstdaccel.h>
#include <kurldrag.h>
#include <kglobalsettings.h>


bool KLSHistoryCombo::items_saved_ = false;


KLSHistoryCombo::KLSHistoryCombo(QWidget *parent, const char *name)
        : KHistoryCombo(parent, name)
{
    setMaxCount(KLSConfig::maxCountComboUrl());
    
    setDuplicatesEnabled(false);
    setAutoCompletion(false);

    connect(this, SIGNAL(activated(const QString& )),
            this, SLOT(addToHistory(const QString& )));
}

KLSHistoryCombo::~KLSHistoryCombo()
{}

void KLSHistoryCombo::init()
{
    loadItems();
}

void KLSHistoryCombo::saveItems()
{
    if(items_saved_)
        return;

    QStringList items = historyItems();

    KLSConfig::setComboUrlHistory(items);
    KLSConfig::writeConfig();
    
    items_saved_ = true;
}

void KLSHistoryCombo::loadItems()
{
    clear();
    
    QStringList items = KLSConfig::comboUrlHistory();

    bool block = signalsBlocked();
    blockSignals( true );

    setHistoryItems(items);
    blockSignals(block);

    completionObject()->setItems(items);

    setCompletionMode(KGlobalSettings::completionMode());
}

bool KLSHistoryCombo::eventFilter( QObject *o, QEvent *ev )
{
    // Handle Ctrl+Del/Backspace etc better than the Qt widget, which always
    // jumps to the next whitespace.
    QLineEdit *edit = lineEdit();
    if ( o == edit )
    {
        int type = ev->type();
        if ( type == QEvent::KeyPress )
        {
            QKeyEvent *e = static_cast<QKeyEvent *>( ev );

            if ( e->key() == Key_Return || e->key() == Key_Enter )
            {
                //m_modifier = e->state();
                return false;
            }

            int delete_word_back = KStdAccel::deleteWordBack().keyCodeQt();
            int delete_word_forward = KStdAccel::deleteWordForward().keyCodeQt();

            if ( KKey( e ) == KKey(delete_word_back) ||
                    KKey( e ) == KKey(delete_word_forward) ||
                    ((e->state() & ControlButton) &&
                     (e->key() == Key_Left || e->key() == Key_Right) ) )
            {
                selectWord(e);
                e->accept();
                return true;
            }
        }

        else if ( type == QEvent::MouseButtonDblClick )
        {
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
    QValueList<QChar> chars;
    chars << QChar('/') << QChar('.') << QChar('?') << QChar('#') << QChar(':');
    bool allow_space_break = true;

    if( e->key() == Key_Left || e->key() == Key_Backspace )
    {
        do
        {
            pos--;
            count++;
            if( allow_space_break && text[pos].isSpace() && count > 1 )
                break;
        }
        while( pos >= 0 && (chars.findIndex(text[pos]) == -1 || count <= 1) );

        if( e->state() & ShiftButton )
        {
            edit->cursorForward(true, 1-count);
        }
        else if(  e->key() == Key_Backspace )
        {
            edit->cursorForward(false, 1-count);
            QString text = edit->text();
            int pos_to_right = edit->text().length() - pos_old;
            QString cut = text.left(edit->cursorPosition()) + text.right(pos_to_right);
            edit->setText(cut);
            edit->setCursorPosition(pos_old-count+1);
        }
        else
        {
            edit->cursorForward(false, 1-count);
        }
    }
    else if( e->key() == Key_Right || e->key() == Key_Delete )
    {
        do
        {
            pos++;
            count++;
            if( allow_space_break && text[pos].isSpace() )
                break;
        }
        while( pos < (int) text.length() && chars.findIndex(text[pos]) == -1 );

        if( e->state() & ShiftButton )
        {
            edit->cursorForward(true, count+1);
        }
        else if(  e->key() == Key_Delete )
        {
            edit->cursorForward(false, -count-1);
            QString text = edit->text();
            int pos_to_right = text.length() - pos - 1;
            QString cut = text.left(pos_old) +
                          (pos_to_right > 0 ? text.right(pos_to_right) : QString() );
            edit->setText(cut);
            edit->setCursorPosition(pos_old);
        }
        else
        {
            edit->cursorForward(false, count+1);
        }
    }
}

#include "klshistorycombo.moc"
