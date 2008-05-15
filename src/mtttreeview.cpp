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
