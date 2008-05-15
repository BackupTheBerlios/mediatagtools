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
	rootData << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
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
    if (!index.isValid())
        return QVariant();

    if ( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) ) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        return item->data(index.column());
    }
    else if ( role == Qt::ForegroundRole ) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if ( item->getColor().isValid() )
            return item->getColor();
        else
            return QVariant();
    }
    else
        return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
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
    emit headerDataChanged(Qt::Horizontal, 0, 7);
}

bool TreeModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    TreeItem *item;

    item = static_cast<TreeItem*>( index.internalPointer() );

    if ( role == Qt::ForegroundRole ) {
        if ( item )
            item->setColor( value.value<QColor>() );
        else {
            //qDebug( "setData::FG::item == NULL" );
            return false;
        }
    }
    else {
        if ( item )
            item->setData( index.column(), value );
        else {
            //qDebug( "setData::ELSE::item == NULL" );
            return false;
        }
    }

    emit dataChanged( index, index );

    return true;
}

bool TreeModel::setData ( const QModelIndex & index, const QList<QVariant> & value, int role )
{
    TreeItem *item;

    item = static_cast<TreeItem*>( index.internalPointer() );
    if ( item )
        item->setData( value );
    else
        return false;

    emit dataChanged( index, index );

    return true;
}

bool TreeModel::insertRows ( int row, int count, const QModelIndex & parent )
{
    TreeItem *parentItem;
    QList<QVariant> list;
    int i;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    beginInsertRows( parent, row, row + count - 1 );
    for ( i=0; i<count; i++)
        parentItem->appendChild( new TreeItem( list, parentItem ) );
    endInsertRows();
}

bool TreeModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    TreeItem *parentItem;
    QList<QVariant> list;
    int i;

    if (!parent.isValid()) {
        parentItem = rootItem;
        qDebug( "rootItem" );
    }
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    beginRemoveRows( parent, row, row + count - 1 );
    parentItem->deleteChildren( row, count );
    endRemoveRows();
}
