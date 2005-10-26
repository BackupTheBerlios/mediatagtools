//
// C++ Interface: 
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ATTMAINWIN_H
#define ATTMAINWIN_H

#include <qdir.h>
#include <qstringlist.h>
#include <qmenubar.h>
#include <qprogressbar.h>

#include "form1.h"

class mttMainWin : public MainForm
{
  Q_OBJECT

public:
  mttMainWin(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~mttMainWin();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
    virtual void slotOpen();
    virtual void slotRemoveTags();
    virtual void slotSaveTags();
    virtual void slotClickOnItem( QListViewItem* item );
    virtual void slotCFormat();

protected:
  /*$PROTECTED_FUNCTIONS$*/
    QDir d;
    QStringList separators;
    QMenuBar *menu;
    QProgressBar progress;

    void populateList( void );
    QString firstUp( QString );

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void slotDisableUsingFormat( bool );
    virtual void slotRenameFiles();
    virtual void slotAbout();
    virtual void slotCorrectCase();
    virtual void slotFirstUp();
    virtual void slotAllUpper();
    virtual void slotAllLower();
    virtual void slotEmptyFields();

};

#endif

