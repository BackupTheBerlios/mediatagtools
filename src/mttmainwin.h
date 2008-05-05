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

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>

#include "ui_mainform.h"
#include "treemodel.h"

class mttMainWin : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  mttMainWin(QWidget* parent = 0 );
  ~mttMainWin();
  /*$PUBLIC_FUNCTIONS$*/
    void addDir( QString );
    void addFile( QString );
    void updateAdvMp3TagTable( QStringList );

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
//     QDir d;
    TreeModel model;

    QString curPath;
    QStringList separators;
    QProgressBar progress;
    bool ignoreChange;
    QString selectedFname;
    QStringList availExtraFrames;
    QStringList xtraFrames;

    void populateList( QDir d );
    QString firstUp( QString );
    QString firstUpSentence( QString );
    bool isTextFrame( QString );
    void saveTags( bool selectedOnly = false );

    static const char x_col = 0;
    static const char field_col = 1;
    static const char field_id_col = 2;
    static const char value_col = 3;

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void slotOpen();
/*    virtual void slotRemoveTags();
    virtual void slotSaveTags();
    virtual void slotSaveSelectedTags();
    virtual void slotCFormat();
    virtual void slotDisableUsingFormat( bool );
    virtual void slotRenameFiles();
    virtual void slotAbout();
    virtual void slotCorrectCase();
    virtual void slotFirstUpSentence();
    virtual void slotFirstUpWords();
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
    virtual void slotTitleChanged( const QString& );
    virtual void slotSelectionChange();
    virtual void slotGenreChanged( const QString& );
    virtual void slotTrackChanged( const QString& );
    virtual void slotCommentChanged( const QString& );
    virtual void slotYearChanged( const QString& );
    virtual void slotAlbumChanged( const QString& );
    virtual void slotArtistChanged( const QString& );*/
    virtual void slotOpenFiles();
/*    virtual void slotCreateTags();
    virtual void slotAdvTagValueChanged( int, int );
    virtual void slotRemoveAdvTags();
    virtual void slotRemoveFiles();
    virtual void slotDroppedUris( QStringList );
    virtual void slotXButtonClickedMP3( int, int );*/
};

#endif
