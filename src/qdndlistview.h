//
// C++ Interface: qdndlistview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QDNDLISTVIEW_H
#define QDNDLISTVIEW_H

#include <qlistview.h>

/**
	@author Theofilos Intzoglou <int.teo@gmail.com>
*/
class QDNDListView : public QListView
{
Q_OBJECT

public:
    QDNDListView();
    QDNDListView( QWidget *w, const char *name ) : QListView( w, name ) {};

    ~QDNDListView();

    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

signals:
    void droppedUris( QStringList );

};

#endif
