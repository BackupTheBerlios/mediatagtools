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
    if (parent.isValid()) {
        //std::cout << "columnCount: parent is valid!" << std::endl;
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }
    else {
		//std::cout << "columnCount: parent not valid! CC=" << rootItem->columnCount() << std::endl;
        return rootItem->columnCount();
    }
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        //std::cout << "data: index not valid" << std::endl;
        //qDebug("Index not valid!");
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        //std::cout << "data: role not DisplayRole" << std::endl;
        //qDebug("Role!");
        return QVariant();
    }

    //std::cout << "ok!" << std::endl;
    //qDebug( "ok" );
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        //std::cout << "flags: !index.isValid" << std::endl;
        return 0;
    }

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
    if (!hasIndex(row, column, parent)) {
        //std::cout << "!hasIndex" << std::endl;
        return QModelIndex();
    }

	//std::cout << "index ok!" << std::endl;
    TreeItem *parentItem;

	if (!parent.isValid()) {
		//std::cout << "returned rootItem" << std::endl;
        parentItem = rootItem;
	}
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
	//std::cout << "childItem: " << childItem << std::endl;
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        //std::cout << "parent: !isValid" << std::endl;
        return QModelIndex();
    }

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

    //std::cout << "Here!";
    if (!parent.isValid()) {
		//std::cout << "rowCount: parent !isValid" <<std::endl;
        parentItem = rootItem;
    }
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    //std::cout << parentItem->childCount() << std::endl;
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
