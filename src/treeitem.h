//
// C++ Interface: treeitem
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QColor>

#include "mttfile.h"

class TreeItem
{
public:
    //TreeItem( TreeItem *parent = 0 );
    TreeItem(const QList<QVariant> &data, TreeItem *parent = NULL);
    ~TreeItem();

    void appendChild(TreeItem *child);
    void deleteChildren( int row, int count );

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    void setData( const QList<QVariant> &d );
    void setData( int column, QVariant d );
    QVariant data( int column ) const;
    int row() const;
    TreeItem *parent();
    void setColor( QColor );
    QColor getColor( void );
    void setItemChanged( bool );
    bool isItemChanged( void );
    void setFile( mttFile* );
    mttFile *getFile( void );

private:
    QList<TreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItem *parentItem;

    QColor itemColor;
    bool itemChanged;

    mttFile *mfile;
};

#endif
