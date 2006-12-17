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

#include <qlabel.h>
#include <qpainter.h>
#include <qpicture.h>

#include "mttaboutdialog.h"
#include "config.h"
#ifndef RELEASE
#include "revision.h"
#endif

mttAboutDialog::mttAboutDialog(QWidget* parent, const char* name, bool modal, WFlags fl)
: AboutDialog(parent,name, modal,fl)
{
    ProgNameLabel->setText( QString( "<p align=\"center\"><b><h3>Media Tag Tools v." ) + VERSION + "</h3></b></p>" );
#ifndef RELEASE
    ProgNameLabel->setText( QString( "<p align=\"center\"><b><h3>Media Tag Tools v." ) + VERSION + "-" + RV_SNAPSHOT_VERSION + "</h3></b></p>" );
#endif
    QPicture  pic;
    pic.load( PREFIX + QString( "/share/mediatagtools/icons/logo.svg" ), "svg" );     // load picture
    QPainter  p;
    p.begin( LogoFrame );          // paint in myWidget
    p.drawPicture( pic );          // draw the picture
    p.end();                       // painting done
}

mttAboutDialog::~mttAboutDialog()
{
}

/*$SPECIALIZATION$*/


