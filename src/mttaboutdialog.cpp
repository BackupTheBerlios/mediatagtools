//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QtUiTools/QtUiTools>
#include <QVBoxLayout>

#include "mttaboutdialog.h"
#include "config.h"
#ifndef RELEASE
#include "revision.h"
#endif

mttAboutDialog::mttAboutDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent,f)
{
    QUiLoader loader;

    QFile file(":/forms/about.ui");
    file.open(QFile::ReadOnly);
    QWidget *formWidget = loader.load(&file, this);
    file.close();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(formWidget);
    setLayout(layout);
}

