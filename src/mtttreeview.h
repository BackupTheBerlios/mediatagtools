//
// C++ Interface: mtttreeview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MTTTREEVIEW_H
#define MTTTREEVIEW_H

#include <QtGui/QTreeView>

class mttTreeView : public QTreeView
{
Q_OBJECT

public:
    mttTreeView( QWidget * parent = 0 );

signals:
    void rightMouseButtonReleased(void);

protected:
    void mouseReleaseEvent ( QMouseEvent * event );
};

#endif
