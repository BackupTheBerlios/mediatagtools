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

#include <QList>
#include <QVariant>

#include "mttfile.h"

class TreeItem
{
public:
    TreeItem( TreeItem *parent = 0 );
    TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    void setData( QList<QVariant> );
    void setData( int column, QVariant d ); //TODO: Implementation
    QVariant data( int column ) const;
    int row() const;
    TreeItem *parent();

private:
    QList<TreeItem*> childItems;
    QList<QVariant> itemData;
    TreeItem *parentItem;

    mttFile *mfile;
};

#endif
