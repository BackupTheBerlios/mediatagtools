//
// C++ Implementation: mttitemdelegate
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QtGui/QKeyEvent>
#include "mttitemdelegate.h"

mttItemDelegate::mttItemDelegate( QObject *parent ) : QItemDelegate( parent )
{
}


mttItemDelegate::~mttItemDelegate()
{
}

bool mttItemDelegate::editorEvent( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index )
{
	if ( event->type() == QEvent::KeyPress ) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if ( keyEvent->key() == Qt::Key_Tab ) {
			return true;
		}
		else
			return false;
	}
	else
		return false;
 
}
