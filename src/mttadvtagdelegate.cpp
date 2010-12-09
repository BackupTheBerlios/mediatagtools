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

#include <QComboBox>
#include <QMessageBox>
#include <QListView>
#include <QHeaderView>
#include "mttmainwin.h"
#include "mttadvtagdelegate.h"
#include "mp3extraframes.h"

mttAdvTagDelegate::mttAdvTagDelegate(QObject* parent): QStyledItemDelegate(parent)
{
}

QWidget* mttAdvTagDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0) {
	QComboBox *combo;
	QStringList list;

	for (int i=0; i<EF_NUM; i++)
	    list << extraFrames[i][1];
	
	combo = new QComboBox(parent);
	combo->setEditable(true);
	combo->setModel(((mttMainWin*)parent->parent()->parent()->parent()->parent())->getMp3FrameModel());
	combo->setModelColumn(1);
	combo->setDuplicatesEnabled(false);
	//combo->addItems(list);
	combo->setFrame(true);
	combo->setAutoFillBackground(true);
	QTableView *tv;
	tv = new QTableView(combo);
	tv->horizontalHeader()->setHidden(true);
	tv->verticalHeader()->setHidden(true);
	tv->horizontalHeader()->setStretchLastSection(true);
	tv->setSelectionBehavior(QAbstractItemView::SelectRows);
	tv->setMinimumWidth(parent->width());
	combo->setView(tv);
	tv->resizeColumnToContents(0);
	// TODO: Add Validator for invalid texts
	return combo;
    }
    else
	if (index.sibling(index.row(),0).data().toString().compare(tr("Add tag")) == 0) {
	    QMessageBox::information(0,tr("Information"),tr("Please try to select a type of tag from the first column and then add its value!"));
	    return 0;
	}
	else
	    return QStyledItemDelegate::createEditor(parent, option, index);
}

void mttAdvTagDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == 0) {
	QComboBox *combo = static_cast<QComboBox*>(editor);
	int value = combo->findText(index.data().toString());
	if (value != -1) {
	    combo->setCurrentIndex(value);
	}
	else
	    combo->setCurrentIndex(0);
    }
    else
	QStyledItemDelegate::setEditorData(editor, index);
}

void mttAdvTagDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 0) {
	QComboBox *combo = static_cast<QComboBox*>(editor);
	model->setData( index, combo->currentText(), Qt::EditRole );
    }
    else
	QStyledItemDelegate::setModelData(editor, model, index);
}

void mttAdvTagDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0) {
	editor->setGeometry(option.rect);
    }
    else
	QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

