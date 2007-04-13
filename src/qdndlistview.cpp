//
// C++ Implementation: qdndlistview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "qdndlistview.h"
#include "qdragobject.h"

QDNDListView::QDNDListView()
{
}

QDNDListView::~QDNDListView()
{
}

void QDNDListView::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept(
        QUriDrag::canDecode(event)
    );
}

void QDNDListView::dropEvent(QDropEvent* event)
{
    QStringList uris;

    if ( QUriDrag::decodeToUnicodeUris(event, uris) ) {
        emit droppedUris( uris );
    }
}
