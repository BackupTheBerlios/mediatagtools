//
// C++ Implementation: qclineedit
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "qclineedit.h"

QCLineEdit::QCLineEdit( QWidget *parent, const char *name = 0 )
 : QLineEdit( parent, name )
{
}

QCLineEdit::~QCLineEdit()
{
}

void QCLineEdit::keyReleaseEvent( QKeyEvent * e )
{
    switch( e->key() ){
        case Qt::Key_Up: // Cursor Up
            emit previousEntry();
            break;
        case Qt::Key_Down: // Cursor Down
            emit nextEntry();
            break;
        case Qt::Key_Prior: // PageUp
            emit previousPage();
            break;
        case Qt::Key_Next: // PageDown
            emit nextPage();
            break;
        default:
            e->ignore();
    }
}
