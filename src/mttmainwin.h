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

#include <QDir>
#include <QStringList>
#include <QStandardItemModel>
#include <QMenuBar>
#include <QProgressBar>
#include <QDockWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QListView>
#include <QLabel>
#include <QTreeView>
#include <QCheckBox>

#include "ui_mainform.h"
#include "mtttreeview.h"

class mttMainWin : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  mttMainWin(QWidget* parent = 0 );
  ~mttMainWin();
  /*$PUBLIC_FUNCTIONS$*/
    void addDir( QString );
    void addFile( QString& );
    void updateAdvMp3TagTable( QStringList );

public slots:
  /*$PUBLIC_SLOTS$*/

private:
    QStandardItemModel treeModel;
	QStandardItemModel renumModel;
	QListView *listView;
    mttTreeView *treeView;

    // Needed for dockEdit
	QLineEdit *titleEdit,*artistEdit,*albumEdit,*commentEdit,*yearEdit,*trackEdit;
	QComboBox *genreEdit;

    // Needed for dockRenum
	QLabel *lengthLabel, *bitrateLabel, *sampleRateLabel, *channelLabel;

    // Needed for dockFormat
    QComboBox *format;
    QCheckBox *autoUpd;
    QLabel *titleLabel, *artistLabel, *albumLabel, *commentLabel, *yearLabel, *trackLabel, *genreLabel;

protected:
  /*$PROTECTED_FUNCTIONS$*/
//     QDir d;
    QStringList knownFiletypes;
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
    void saveTags( bool );
    void changeColumnText( int, const QString & );
    void setTagChanged( const QModelIndex &, bool changed = true );

    static const char x_col = 0;
    static const char field_col = 1;
    static const char field_id_col = 2;
    static const char value_col = 3;

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void slotOpen();
    virtual void slotOpenFiles();
    virtual void slotRemoveTags();
    /*virtual void slotSaveTags();
    virtual void slotSaveSelectedTags();
    virtual void slotCFormat();
    virtual void slotDisableUsingFormat( bool );
    virtual void slotRenameFiles();
    virtual void slotAbout();
    virtual void slotCorrectCase();*/
    virtual void slotFirstUpSentence();
    virtual void slotFirstUpWords();
    virtual void slotAllUpper();
    virtual void slotAllLower();
    //virtual void slotEmptyFields();
    //virtual void slotFixTags();
    virtual void slotCommentEnter();
    //virtual void slotGenreEnter(); // Only available through subclassing I'm afraid :-(
    virtual void slotYearEnter();
    virtual void slotAlbumEnter();
    virtual void slotArtistEnter();
    virtual void slotTitleEnter();
    /*virtual void slotNextEntry();
    virtual void slotPreviousEntry();
    virtual void slotPreviousPage();
    virtual void slotNextPage();*/
    virtual void slotSelectionChange( const QModelIndex&, const QModelIndex& );
    virtual void slotSelectionChange( const QItemSelection&, const QItemSelection& );
    virtual void slotTitleChanged( const QString& );
    virtual void slotGenreChanged( const QString& );
    virtual void slotTrackChanged( const QString& );
    virtual void slotCommentChanged( const QString& );
    virtual void slotYearChanged( const QString& );
    virtual void slotAlbumChanged( const QString& );
    virtual void slotArtistChanged( const QString& );
/*    virtual void slotCreateTags();
    virtual void slotAdvTagValueChanged( int, int );
    virtual void slotRemoveAdvTags();*/
    virtual void slotRemoveFiles();
/*    virtual void slotDroppedUris( QStringList );
    virtual void slotXButtonClickedMP3( int, int );*/
    virtual void slotUpButtonClicked();
    virtual void slotDownButtonClicked();
    virtual void slotRenumButtonClicked( bool );
    virtual void slotFormatChanged( const QString& );
};

#endif
