#include "renummodel.h"

RenumModel::RenumModel(void)
{
}

RenumModel::~RenumModel(void)
{
}

Qt::ItemFlags RenumModel::flags ( const QModelIndex & index ) const
{
	//qDebug( QString( QString("flags") + QString( index.isValid()?"true":"false" ) ).toUtf8().constData() );
	//if ( !index.isValid() )
	//	return 0;

	return ( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
}

QVariant RenumModel::data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
{
	//qDebug( "data" );
	//qDebug( QString::number( role ).toUtf8().constData() );
	if ( !index.isValid() )
		return QVariant();
	
	if ( role == Qt::DisplayRole ) {
		if ( index.column() == 0 ) {
			//qDebug( strlist.at( index.row() ).toUtf8().constData() );
			return strlist.at( index.row() );
		}
		else
			return QVariant();
	}

	return QVariant();
}

QVariant RenumModel::headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
{
	if ( role != Qt::DisplayRole )
		return QVariant();

	if ( orientation == Qt::Horizontal ) {
		if ( section == 0 )
			return tr("Track Title");
		else
			return QVariant();
	}
	else if ( orientation == Qt::Vertical )
		return section+1;
	else
		return QVariant();
}

int RenumModel::rowCount ( const QModelIndex & parent = QModelIndex() ) const
{
	if ( parent.isValid() )
		return 0;
	else
		return strlist.count();
}

int RenumModel::columnCount ( const QModelIndex & parent = QModelIndex() ) const
{
	if ( parent.isValid() )
		return 0;
	else
		return 1;
}

void RenumModel::setData( const QModelIndexList & list )
{
	strlist.clear();
	for (int i=(list.count()/8);i<(2*list.count()/8);i++) {
		strlist << list.at(i).data().toString();
	}
	reset();
}

Qt::DropActions RenumModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool RenumModel::insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() )
{
	//qDebug( "insertRows" );
	if ( parent.isValid() )
		return false;
	else {
		if ( count < 1 || row < 0 || row > strlist.count() )
			return false;
		else {
			for ( int i=0;i<count;i++ )
				strlist.insert( row, "" );
			return true;
		}
	}
}

bool RenumModel::removeRow ( int row, const QModelIndex & parent = QModelIndex() )
{
	return removeRows( row, 0, parent );
}

bool RenumModel::removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() )
{
	//qDebug( "removeRows" );
	if ( parent.isValid() )
		return false;
	else {
		if ( count < 1 || row < 0 || row > strlist.count() )
			return false;
		else {
			for ( int i=0;i<count;i++ )
				strlist.removeAt( row );
			return true;
		}
	}
}

bool RenumModel::setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
{
	//qDebug( "setData" );
	if ( !index.isValid() )
		return false;

	//qDebug( value.toString().toUtf8().constData() );
	if ( role == Qt::EditRole || role == Qt::DisplayRole ) {
		if ( index.row() < 0 || index.row() > strlist.count() || index.column() != 0 )
			return false;
		else {
			strlist.replace( index.row(), value.toString() );
			emit dataChanged( QModelIndex( index.sibling( index.row(), 0 ) ), QModelIndex( index.sibling( index.row(), 0 ) ) );

			return true;
		}
	}
	else
		return false;
}

QModelIndex RenumModel::parent ( const QModelIndex & index ) const
{
	return QModelIndex();
}

QModelIndex RenumModel::index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

	if ( parent.isValid() )
		return QModelIndex();

	if ( column != 0 || row < 0 || row > strlist.count() )
		return QModelIndex();
	else
        return createIndex(row, column, ( void * ) &strlist.at( row ) );
}
