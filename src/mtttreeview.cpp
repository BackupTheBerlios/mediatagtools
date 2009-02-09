//
// C++ Implementation: mtttreeview
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with parent distribution
//
//

#include <QtGui/QMenu>

#include "mtttreeview.h"

mttTreeView::mttTreeView( QWidget *parent )
 : QTreeView( parent )
{
    mttTreeView::parent = parent;
}


mttTreeView::~mttTreeView()
{
}


void mttTreeView::contextMenuEvent( QContextMenuEvent *e )
{
    QMenu menu, corCaseMenu;

    corCaseMenu.setTitle( "Correct Case" );
    corCaseMenu.addAction( tr( "First letter up (first word)" ), parent, SLOT(slotFirstUpSentence()) );
    corCaseMenu.addAction( tr( "First letter up (each word)" ), parent, SLOT(slotFirstUpWords()) );
    corCaseMenu.addAction( tr( "All uppercase" ), parent, SLOT(slotAllUpper()) );
    corCaseMenu.addAction( tr( "All lowercase" ), parent, SLOT(slotAllLower()) );

    menu.addAction( tr( "Open folder" ), parent, SLOT(slotOpen()) );
    menu.addAction( tr( "Add file(s)" ), parent, SLOT(slotOpenFiles()) );
    menu.addAction( tr( "Remove file(s)" ), parent, SLOT(slotRemoveFiles()) );
    menu.addSeparator();
    menu.addAction( tr( "Write tags" ), parent, SLOT(slotSaveTags()) );
    menu.addAction( tr( "Write selected tags only" ), parent, SLOT(slotSaveSelectedTags()) );
    menu.addAction( tr( "Remove tag" ), parent, SLOT(slotRemoveTags()) );
    menu.addSeparator();
    menu.addMenu( &corCaseMenu );
    menu.addAction( tr( "Fix tag (iso->utf8)" ), parent, SLOT(slotFixTags()) );
    menu.exec( e->globalPos() );
}
