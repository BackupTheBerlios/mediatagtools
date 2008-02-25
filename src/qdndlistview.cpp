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
#include "q3dragobject.h"
//Added by qt3to4:
#include <QDragEnterEvent>
#include <QDropEvent>

QDNDListView::QDNDListView()
{
}

QDNDListView::~QDNDListView()
{
}

void QDNDListView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
         event->acceptProposedEvent();
}

void QDNDListView::dropEvent(QDropEvent* event)
{
    QStringList uris;
    QList <QUrl> urllist;

    if ( event->mimeData()->hasUrls() ) {
        urllist = event->mimeData()->urls();
        for (int i = 0; i < urllist.size(); ++i) {
            uris << urllist.at(i).toString();
        }
        emit droppedUris( uris );
    }
}
