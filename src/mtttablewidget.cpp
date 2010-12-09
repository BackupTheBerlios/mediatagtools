/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "mtttablewidget.h"
#include <QMenu>
#include <QContextMenuEvent>

mttTableWidget::mttTableWidget(QWidget* parent): QTableWidget(parent)
{
    this->parent = parent;
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotItemChanged(QTableWidgetItem*)));
}

void mttTableWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu menu, corCaseMenu;

    menu.addAction( tr( "Add Extra String" ), this, SLOT(slotAdd()) );
    menu.addAction( tr( "Remove Selected String(s)" ), this, SLOT(slotRemoveFiles()) );
    menu.exec( e->globalPos() );
}

void mttTableWidget::slotAdd(void)
{
    QModelIndexList list;
    int count;
    
    list = this->selectedIndexes();
    count = list.count();

    for (int i=0; i<count; i++) {
	if (list.at(i).column() == (columnCount() - 1)) {
	    insertColumn(columnCount());
	}
    }
}

void mttTableWidget::slotItemChanged(QTableWidgetItem *item)
{
    if ((item->column() == 0) && (item->row() == (rowCount() - 1)) &&
	item->data(Qt::DisplayRole).toString().compare(tr("Add tag")) != 0) {
	insertRow(rowCount());
	item->setData(Qt::DecorationRole, QIcon(QString(":/icons/ABC.png")));
    }
    if (item->data(Qt::DisplayRole).toString().isEmpty())
	item->setData(Qt::BackgroundColorRole, Qt::gray);
    else if (item->data(Qt::UserRole).toString().compare(QString("+")) != 0)
	item->setData(Qt::BackgroundColorRole, QColor("#f1e4f2"));
    // TODO: Create a custom QComboBox for the first column that would show a popup with maximum size
    if (item->column()!=0 && !item->text().isEmpty())
	this->resizeColumnToContents(item->column());
}

void mttTableWidget::insertColumn(int column)
{
    int rows;
    
    QTableWidget::insertColumn(column);
    rows = rowCount();
    for (int i=0; i<rows; i++) {
	setItem(i, column, new QTableWidgetItem(QString()));
	item(i, column)->setData(Qt::BackgroundColorRole, Qt::gray);
    }
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(tr("Value ")));
}

void mttTableWidget::insertRow(int row)
{
    int columns;
    
    QTableWidget::insertRow(row);
    //this->setRowHeight(row,this->rowHeight(row)*2);
    columns = columnCount();
    for (int i=0; i<columns; i++) {
	QTableWidgetItem *item;
	item = new QTableWidgetItem(QString());
	if (i == 0) {
	    if (row == (rowCount()-1))
		item->setData(Qt::DisplayRole, tr("Add tag"));
	    item->setData(Qt::DecorationRole, QIcon(QString(":/icons/plus.png")));
	    item->setData(Qt::UserRole, QString("+"));
	}
	else
	    item->setData(Qt::BackgroundColorRole, Qt::gray);
	setItem(row, i, item);
    }
}
