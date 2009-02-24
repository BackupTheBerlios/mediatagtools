//
// C++ Interface: qclineedit
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QCLINEEDIT_H
#define QCLINEEDIT_H

#include <QLineEdit>
//Added by qt3to4:
#include <QKeyEvent>

/**
	@author Theofilos Intzoglou <int.teo@gmail.com>
*/
class QCLineEdit : public QLineEdit
{
Q_OBJECT

public:
    QCLineEdit( QWidget*, const char* );

    ~QCLineEdit();

    void keyReleaseEvent( QKeyEvent * );

signals:
    void nextEntry( void );
    void previousEntry( void );
    void nextPage( void );
    void previousPage( void );

};

#endif
