//
// C++ Interface: mtttreeview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MTTTREEVIEW_H
#define MTTTREEVIEW_H

#include <QtGui/QTreeView>
#include <QtGui/QContextMenuEvent>

/**
	@author Theofilos Intzoglou
*/
class mttTreeView : public QTreeView
{
public:
    mttTreeView( QWidget *parent = 0 );

    ~mttTreeView();
    void contextMenuEvent( QContextMenuEvent * );

private:
    QWidget *parent;
};

#endif
