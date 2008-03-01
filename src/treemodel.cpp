//
// C++ Implementation: treemodel
//
// Description: Provides a simple tree model to show how to create and use hierarchical models.
// (Modified Qt4 example file)
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel( QObject *parent )
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootItem = new TreeItem(rootData);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        //qDebug("Index not valid!");
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        //qDebug("Role!");
        return QVariant();
    }

    //qDebug( "ok" );
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

TreeItem *TreeModel::invisibleRootItem( void )
{
    return rootItem;
}

TreeItem *TreeModel::findItem( QString str )
{
    int i;
    TreeItem *tmp;

    for (i=0;i<rootItem->childCount();i++) {
        tmp = rootItem->child( i );
        if ( tmp->data( 0 ).toString() == str )
            return tmp;
    }
    return NULL;
}

void TreeModel::setHorizontalHeaderLabels( QList<QVariant> strl )
{
    rootItem->setData( strl );
}
