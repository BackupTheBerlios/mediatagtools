//
// C++ Implementation: treeitem
//
// Description: A container for items of data supplied by the simple tree model. (Modified Qt4 example file)
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QtCore/QStringList>

#include "treeitem.h"

/*TreeItem::TreeItem( TreeItem *parent )
{
    parentItem = parent;
}*/

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if ( column <= ( itemData.size() - 1 ) )
        return itemData.value(column);
    else
        return new QVariant();
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

void TreeItem::setData( const QList<QVariant> &d )
{
    itemData.clear();
    itemData += d;
}

void TreeItem::setData( int column, QVariant d )
{
    if ( ( column >= 0 ) && ( column < columnCount() ) )
        itemData.replace( column, d );
}
