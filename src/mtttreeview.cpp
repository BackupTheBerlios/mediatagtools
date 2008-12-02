//
// C++ Implementation: mtttreeview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QtGui/QMouseEvent>

#include "mtttreeview.h"

mttTreeView::mttTreeView( QWidget * parent ) : QTreeView( parent )
{
}

void mttTreeView::mouseReleaseEvent ( QMouseEvent * event )
{
    if ( event->button() == Qt::RightButton )
    {
        event->accept();
        emit rightMouseButtonReleased();
    }
}

// void mttTreeView::keyReleaseEvent( QKeyEvent * event )
// {
// 	// Mental note: I don't think that this is the place for defining the use of tab. Try using a delegate instead
// 	/*if ( event->key() == Qt::Key_Tab )
// 	{
// 			event->accept();
// 
// 			QList<QModelIndex> list;
// 
// 			list = selectionModel()->selectedIndexes();
// 			selectionModel()->select( list.at(0).sibling( list.at(0).row(), list.at(0).column() + 1 ), QItemSelectionModel::ClearAndSelect );
// 	}*/
// }
