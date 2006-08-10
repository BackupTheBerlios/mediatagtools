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

protected:
  /*$PROTECTED_FUNCTIONS$*/
//     QDir d;
    QString curPath;
    QStringList separators;
    QProgressBar progress;
    QString LZ1, LZ2;
    bool LZ;
    QString selectedFname;

    void populateList( QDir d );
    QString firstUp( QString );
    bool isTextFrame( QString );
    void saveTags( bool selectedOnly = false );

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void slotOpen();
    virtual void slotRemoveTags();
    virtual void slotSaveTags();
    virtual void slotSaveSelectedTags();
    virtual void slotCFormat();
    virtual void slotDisableUsingFormat( bool );
    virtual void slotRenameFiles();
    virtual void slotAbout();
    virtual void slotCorrectCase();
    virtual void slotFirstUp();
    virtual void slotAllUpper();
    virtual void slotAllLower();
    virtual void slotEmptyFields();
    virtual void slotLVRightMenu();
    virtual void slotFixTags();
    virtual void slotCommentEnter();
    virtual void slotGenreEnter();
    virtual void slotYearEnter();
    virtual void slotAlbumEnter();
    virtual void slotArtistEnter();
    virtual void slotTitleEnter();
    virtual void slotNextEntry();
    virtual void slotPreviousEntry();
    virtual void slotPreviousPage();
    virtual void slotNextPage();
    virtual void slotTitleChanged( QString );
    virtual void slotSelectionChange();
    virtual void slotGenreChanged( const QString& );
    virtual void slotTrackChanged( QString );
    virtual void slotCommentChanged( QString );
    virtual void slotYearChanged( QString );
    virtual void slotAlbumChanged( QString );
    virtual void slotArtistChanged( QString );
    virtual void slotOpenFiles();
    virtual void slotCreateTags();

};

#endif
