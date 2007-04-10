//
// C++ Interface: klshistorycombo
//
// Description:
//
//
// Author: Paulo Moura Guedes <moura@kdewebdev.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KLSHISTORYCOMBO_H
#define KLSHISTORYCOMBO_H

#include <khistorycombobox.h>
class QKeyEvent;
class KConfig;

/**
@author Paulo Moura Guedes
Based on KonqCombo
*/
class KLSHistoryCombo : public KHistoryComboBox
{
    Q_OBJECT

public:
    explicit KLSHistoryCombo(QWidget* parent);
    ~KLSHistoryCombo();

    void addCurrentItem(QString const& text);
    
    void init();
    void loadItems();
    void saveItems();

protected:
    virtual bool eventFilter(QObject* o, QEvent* ev);
    void selectWord(QKeyEvent* e);
    
private:
    static bool items_saved_;
};

#endif
