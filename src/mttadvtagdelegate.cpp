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

#include "mttadvtagdelegate.h"
#include "mp3extraframes.h"
#include <QComboBox>

mttAdvTagDelegate::mttAdvTagDelegate(QObject* parent): QItemDelegate(parent)
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
	combo->setDuplicatesEnabled(false);
	combo->addItems(list);
	// TODO: Add Validator for invalid texts
	return combo;
    }
    else
	return QItemDelegate::createEditor(parent, option, index);
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
	QItemDelegate::setEditorData(editor, index);
}

void mttAdvTagDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 0) {
	QComboBox *combo = static_cast<QComboBox*>(editor);
	model->setData( index, combo->currentText(), Qt::EditRole );
    }
    else
	QItemDelegate::setModelData(editor, model, index);
}

void mttAdvTagDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0) {
	editor->setGeometry(option.rect);
    }
    else
	QItemDelegate::updateEditorGeometry(editor, option, index);
}

