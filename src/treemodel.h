//
// C++ Interface: treemodel
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel( QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    TreeItem *invisibleRootItem( void );
    TreeItem *findItem( QString );
    void setHorizontalHeaderLabels( QList<QVariant> );

    bool setData ( const QModelIndex & index, const QVariant & value,
                   int role = Qt::EditRole );
    bool setData ( const QModelIndex & index, const QList<QVariant> & value,
                   int role = Qt::EditRole );
    bool insertRows ( int row, int count,
                      const QModelIndex & parent = QModelIndex() );
    bool removeRows ( int row, int count,
                      const QModelIndex & parent = QModelIndex() );

private:
    TreeItem *rootItem;
};

#endif
