//
// C++ Interface: mttitemdelegate
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MTTITEMDELEGATE_H
#define MTTITEMDELEGATE_H

/**
	@author Theofilos Intzoglou
*/

#include <QtGui/QItemDelegate>

class mttItemDelegate : QItemDelegate {
public:
    mttItemDelegate( QObject *parent = 0 );
	bool editorEvent( QEvent *, QAbstractItemModel *, const QStyleOptionViewItem &, const QModelIndex & );

    ~mttItemDelegate();

};

#endif
