#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>

class RenumModel :
	public QAbstractTableModel
{
public:
	RenumModel();
	~RenumModel();

	Qt::ItemFlags flags ( const QModelIndex & ) const;
	QVariant data ( const QModelIndex &, int ) const;
	QVariant headerData ( int, Qt::Orientation, int ) const;
	int rowCount ( const QModelIndex & ) const;
	int columnCount ( const QModelIndex & ) const;
	void setData( const QModelIndexList & );
	Qt::DropActions supportedDropActions() const;
	bool insertRows ( int, int, const QModelIndex & );
	bool removeRow ( int, const QModelIndex & );
	bool removeRows ( int, int, const QModelIndex & );
	bool setData ( const QModelIndex &, const QVariant &, int );
	QModelIndex parent ( const QModelIndex & index ) const;
	QModelIndex index ( int, int, const QModelIndex & ) const;

private:
	QStringList strlist;
};
