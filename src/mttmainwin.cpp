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

#include <iostream>
#include <QFileDialog>
#include <QPixmap>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QList>

#include <tag.h>
#include <tlist.h>
#include <tstring.h>
#include <id3v2framefactory.h>
#include <id3v2tag.h>
#include <textidentificationframe.h>

//#include "qclineedit.h"
//#include "qdndlistview.h"
#include "mttmainwin.h"
#include "mttfile.h"
//#include "mttcfdialog.h"
//#include "mttaboutdialog.h"
#include "config.h"
//#include "x.xpm"
#include "treeitem.h"

#ifndef RELEASE
#include "revision.h"
#endif

//#include "genres.h"
//#include "mp3extraframes.h"

mttMainWin::mttMainWin(QWidget* parent) : QMainWindow( parent )
{
    setupUi( this );
    int i;
    /*QStringList strlst( "<Empty>" );

    ignoreChange = false;

    GenGenreCB->insertStrList( genres );
    for ( i=0; i<EF_NUM; i++ ) {
        strlst.append( extraFrames[i][1] );
    }
    strlst.sort();
    availExtraFrames = strlst;*/

    /*QPixmap pix( ( const char ** ) x_xpm );
    AdvTagTable->setPixmap( 0, x_col, pix );
    AdvTagTable->setColumnReadOnly( x_col, TRUE );
    AdvTagTable->setColumnReadOnly( field_id_col, TRUE );
    AdvTagTable->setItem( 0, field_col, new Q3ComboTableItem( AdvTagTable, strlst ) );
    AdvTagTable->setColumnWidth( x_col, 20 );
    AdvTagTable->setColumnWidth( field_col, 120 );
    AdvTagTable->setColumnWidth( field_id_col, 70 );
    AdvTagTable->setFocusStyle( Q3Table::FollowStyle );
    AdvTagTable->setSelectionMode( Q3Table::Single );*/

    for ( i=0; i<5; i++ ) {
        separators << " - ";
    }
#ifndef RELEASE
    setWindowTitle( windowTitle() + " - " + RV_SNAPSHOT_VERSION );
#endif

    /*tabWidget->removePage( tabWidget->page( 2 ) );
    tabWidget->removePage( tabWidget->page( 2 ) );
//     tabWidget->removePage( tabWidget->page( 1 ) );
    UseDFChkBox->hide();
    comboBox1->hide();
    CleanFButton->hide();
    CreateDirButton->hide();*/

    // Initialization of TreeWidget
    treeView->setModel( &model );
    treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    treeView->setTabKeyNavigation( true );
    QList<QVariant> header;
    header << tr( "Filename" ) << tr( "Title" ) << tr( "Artist" ) << tr( "Album" )
           << tr( "Year" ) << tr( "Genre" ) << tr( "Comment" ) << tr( "Track" );
    model.setHorizontalHeaderLabels( header );

    // Progress bar & Status Bar
    progress.setRange( 0, 100 );
    progress.setValue( 100 );
    statusBar()->showMessage( tr( "Ready" ) );

    // Signal & Slots connections for menubar
    actionOpen_folder->setShortcut( tr( "Ctrl+O" ) );
    actionExit->setShortcut( tr( "Alt+X" ) );
    connect(actionOpen_folder, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(actionOpen_files, SIGNAL(triggered()), this, SLOT(slotOpenFiles()));
}

mttMainWin::~mttMainWin()
{
}

void mttMainWin::addDir( QString str )
{
    QDir d;

    d.setPath( str );
    d.setFilter( QDir::Files | QDir::Readable );
    d.setNameFilters( QString( "*.mp3;*.MP3;*.ogg;*.OGG;*.flac;*.FLAC" ).split(';') );

    if ( d.exists() && d.isReadable() ) {
        curPath = d.path();
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        populateList( d );
        QApplication::restoreOverrideCursor();
    }
    selectedFname = "";
}

void mttMainWin::addFile( QString fname )
{
    //mttFile *li;
    //TagLib::Tag *t;
    TreeItem *ti, *father;

    //li = new mttFile();

    father = model.findItem( curPath );
    //qDebug( curPath.toUtf8().constData() );
    //qDebug( fname.toUtf8().constData() );
    if ( father ) {
        //qDebug("true");
        QList<QVariant> list;
        list << fname;
        ti = new TreeItem( list, father );
        father->appendChild( ti );
    }
    else {
        QList<QVariant> list;
        list << curPath;
        ti = new TreeItem( list, model.invisibleRootItem() );
        model.invisibleRootItem()->appendChild( ti );
    }
    //li->setText( 0, fname.right( fname.length() - curPath.length() - 1 ) );
    //qDebug( QString( fname.right( fname.length() - curPath.length() - 1 ).utf8() ) );
    /*if ( QFile::exists( d.path() + "/" + *it ) )
        qDebug( "exists" );
    else
        qDebug( "doesn't exist" );*/
/*    li->Open( fname );
    t = li->getTag();
    if ( t ) {
        li->setText( 1, TStringToQString( t->title() ) );
        li->setText( 2, TStringToQString( t->artist() ) );
        li->setText( 3, TStringToQString( t->album() ) );
        li->setText( 4, QString::number( t->year() ) );
        li->setText( 5, TStringToQString( t->genre() ) );
        li->setText( 6, TStringToQString( t->comment() ) );
        li->setText( 7, QString::number( t->track() ) );
    }
    else
        qDebug( "tag = NULL" );*/
}

void mttMainWin::slotOpen()
{
    QFileDialog fd;
    QDir d;
    bool done = false;

    d.setFilter( QDir::Files | QDir::Readable );
    d.setNameFilters( QString( "*.mp3;*.MP3;*.ogg;*.OGG;*.flac;*.FLAC" ).split(';') );

    fd.setFileMode( QFileDialog::Directory );
    QStringList filters = fd.filters();
    filters << tr( "Audio Files (*.mp3 *.ogg *.flac *.MP3 *.OGG *.FLAC)" );
    fd.setFilters(filters);
    fd.setDirectory( curPath );
    while (!done) {
        if ( fd.exec() == QDialog::Accepted ) {
            //qDebug( fd.selectedFile().utf8() );
            d.setPath( QString( fd.selectedFiles().at( 0 ) ) );
            if ( d.exists() )
                done = true;
            else
                QMessageBox::critical( this, tr( "Error" ), tr( QString( "Dir " + fd.selectedFiles().at( 0 ) + " not found!" ).toUtf8().constData() ), QMessageBox::Ok, QMessageBox::NoButton );
        }
        else
            return;
    }

    curPath = d.path();
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    //model.removeRows( 0, model.rowCount() );
    populateList( d );
    QApplication::restoreOverrideCursor();
    selectedFname = "";
    treeView->show();

    int i;
    TreeItem *tmp,*tmp2,*rootItem;

    rootItem = model.invisibleRootItem();
    std::cout << rootItem->childCount() << std::endl;
    for (i=0;i<rootItem->childCount();i++) {
        tmp = rootItem->child( i );
        //qDebug( tmp->data( 0 ).toString().toUtf8().constData() );
        int j;
        std::cout << tmp->childCount() << std::endl;
        for (j=0;j<tmp->childCount();j++) {
            tmp2 = tmp->child( j );
            //qDebug( tmp2->data( 0 ).toString().toUtf8().constData() );
        }
    }

}

void mttMainWin::slotOpenFiles()
{
    QFileDialog fd;
    QStringList files;

    int count = 0, current = 1;

    fd.setFileMode( QFileDialog::ExistingFiles );
    QStringList filters = fd.filters();
    filters << tr( "Audio Files (*.mp3 *.ogg *.flac *.MP3 *.OGG *.FLAC)" );
    fd.setFilters(filters);
    fd.setDirectory( curPath );
    if ( fd.exec() == QDialog::Accepted ) {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        statusBar()->showMessage( tr( "Reading tags..." ) );

        files = fd.selectedFiles();
        curPath = fd.directory().path();

        for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
        count++;
        }

        statusBar()->addWidget( &progress );
        progress.setRange( 0, count );
        progress.setValue( 0 );

        for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
            addFile( *it );
            progress.setValue( current++ );
        }
        statusBar()->removeWidget( &progress );
        statusBar()->showMessage( tr( "Done" ) );
        QApplication::restoreOverrideCursor();
    }
    else
        return;

    selectedFname = "";
}

void mttMainWin::populateList( QDir d )
{
    QStringList fnames;
    int current, count;

    d.refresh();
    fnames = d.entryList();
    count = d.count();
    current = 1;

    statusBar()->addWidget( &progress );
    progress.setRange( 0, count );
    progress.setValue( 0 );
    statusBar()->showMessage( tr( "Reading tags..." ) );

    for ( QStringList::Iterator it = fnames.begin(); it != fnames.end(); ++it ) {
        addFile( curPath + "/" + *it );
        progress.setValue( current++ );
    }

    statusBar()->removeWidget( &progress );
    statusBar()->showMessage( tr( "Done" ) );
}

// void mttMainWin::slotSaveTags()
// {
//     saveTags( false );
// }
// 
// void mttMainWin::slotSaveSelectedTags()
// {
//     saveTags( true );
// }
// 
// void mttMainWin::slotRemoveTags()
// {
//     int count, current, button;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( it.current() ) {
//         button = QMessageBox::question( this, tr( "Remove tags? -- Media Tag Tools" ), tr( "Are you sure you want to completely remove the tag(s)?" ), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape );
// 
//         if ( button == QMessageBox::Yes ) {
//             QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//             statusBar()->message( tr( QString( "Removing tags..." ) ) );
//             count = GenListView->childCount();
//             current = 1;
//             progress.show();
//             progress.setProgress( 0, count );
//             while ( it.current() ) {
//                 ( (mttFile *) it.current() )->removeTag();
//                 TagLib::Tag *t;
// 
//                 // Show tag info
//                 t = (( mttFile *) it.current() )->getTag();
//                 if ( t ) {
//                     it.current()->setText( 1, TStringToQString( t->title() ) );
//                     it.current()->setText( 2, TStringToQString( t->artist() ) );
//                     it.current()->setText( 3, TStringToQString( t->album() ) );
//                     it.current()->setText( 4, QString::number( t->year() ) );
//                     it.current()->setText( 5, TStringToQString( t->genre() ) );
//                     it.current()->setText( 6, TStringToQString( t->comment() ) );
//                     it.current()->setText( 7, QString::number( t->track() ) );
//                 }
//                 else {
//                     it.current()->setText( 1, "" );
//                     it.current()->setText( 2, "" );
//                     it.current()->setText( 3, "" );
//                     it.current()->setText( 4, "" );
//                     it.current()->setText( 5, "" );
//                     it.current()->setText( 6, "" );
//                     it.current()->setText( 7, "" );
//                 }
// 
//                 ++it;
//                 progress.setProgress( current++, count );
//             }
// 
//             //GenListView->clear();
//             //populateList();
//             progress.hide();
//             statusBar()->message( tr( QString( "Done" ) ) );
//             QApplication::restoreOverrideCursor();
//         }
//     }
// }
// 
// void mttMainWin::slotCFormat()
// {
//     mttCFDialog cfdialog;
// 
//     cfdialog.setFormat( MCFormatLE->text() );
//     cfdialog.setSeparators( separators );
//     if ( cfdialog.exec() == QDialog::Accepted ) {
//         MCFormatLE->setText( cfdialog.getFormat() );
//         separators = cfdialog.getSeparators();
//     }
//     if ( UseCFChkB->isChecked() ) {
//         slotDisableUsingFormat( false ); // Reset state of fields
//         slotDisableUsingFormat( true ); // and set new state according to format string
//     }
// }
// 
// void mttMainWin::slotDisableUsingFormat( bool cond )
// {
//     if ( cond ) {
//         QString formatstr = MCFormatLE->text();
// 
//         if ( formatstr.find( "<artist>" ) != -1 ) {
//             GenArtistChkB->setChecked( false );
//             GenArtistChkB->setDisabled( true );
//         }
//         if ( formatstr.find( "<album>" ) != -1 ) {
//             GenAlbumChkB->setChecked( false );
//             GenAlbumChkB->setDisabled( true );
//         }
//         if ( formatstr.find( "<title>" ) != -1 ) {
//             GenTitleChkB->setChecked( false );
//             GenTitleChkB->setDisabled( true );
//         }
//         if ( formatstr.find( "<year>" ) != -1 ) {
//             GenYearChkB->setChecked( false );
//             GenYearChkB->setDisabled( true );
//         }
//         if ( formatstr.find( "<comment>" ) != -1 ) {
//             GenCommentChkB->setChecked( false );
//             GenCommentChkB->setDisabled( true );
//         }
//         if ( formatstr.find( "<track>" ) != -1 ) {
//             GenTrackChkB->setChecked( false );
//             GenTrackChkB->setDisabled( true );
//         }
//     }
//     else {
//             if ( !GenArtistChkB->isEnabled() ) {
//                 GenArtistChkB->setDisabled( false );
//                 GenArtistChkB->setChecked( true );
//             }
//             if ( !GenAlbumChkB->isEnabled() ) {
//                 GenAlbumChkB->setDisabled( false );
//                 GenAlbumChkB->setChecked( true );
//             }
//             if ( !GenTitleChkB->isEnabled() ) {
//                 GenTitleChkB->setDisabled( false );
//                 GenTitleChkB->setChecked( true );
//             }
//             if ( !GenYearChkB->isEnabled() ) {
//                 GenYearChkB->setDisabled( false );
//                 GenYearChkB->setChecked( true );
//             }
//             if ( !GenCommentChkB->isEnabled() ) {
//                 GenCommentChkB->setDisabled( false );
//                 GenCommentChkB->setChecked( true );
//             }
//             if ( !GenTrackChkB->isEnabled() ) {
//                 GenTrackChkB->setDisabled( false );
//                 GenTrackChkB->setChecked( true );
//             }
//     }
// }
// 
// void mttMainWin::slotRenameFiles()
// {
//     int count, current;
//     int noall = false, abort = false;
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//     statusBar()->message( tr( QString( "Renaming Files..." ) ) );
//     count = GenListView->childCount();
//     current = 1;
//     progress.show();
//     progress.setProgress( 0, count );
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     while ( !abort && it.current() ) {
//         QString newfname, path, ext, cformat;
//         TagLib::Tag *t;
// 
//         t = ( (mttFile *) it.current() )->getTag();
//         path = ( (mttFile *) it.current() )->getFName();
//         ext = path;
//         //qDebug( "filename:" + path );
//         path.truncate( path.findRev( "/" ) );
//         ext = ext.right( ext.length() - ext.findRev( "." ) );
//         cformat = MCFormatLE->text();
// 
//         if ( t && ( cformat != "" ) ) {
//             newfname = "";
//             bool done = false;
//             while ( !done ) {
//                 if ( cformat.startsWith( "<artist>" ) )
//                     newfname += TStringToQString( t->artist() );
//                 if ( cformat.startsWith( "<album>" ) )
//                     newfname += TStringToQString( t->album() );
//                 if ( cformat.startsWith( "<title>" ) )
//                     newfname += TStringToQString( t->title() );
//                 if ( cformat.startsWith( "<track>" ) ) {
//                     if ( t->track() < 10 )
//                         newfname += '0';
//                     newfname += QString::number( t->track() );
//                 }
//                 if ( cformat.startsWith( "<year>" ) )
//                     newfname += QString::number( t->year() );
//                 if ( cformat.startsWith( "<comment>" ) )
//                     newfname += TStringToQString( t->comment() );
//                 cformat.remove( 0, cformat.find( ">" ) + 1 );
// 
//                 if ( cformat == "" )
//                     done = true;
//                 else {
//                     newfname += cformat.left( cformat.find( "<" ) );
//                     cformat.remove( 0, cformat.find( "<" ) );
//                     if ( cformat == "" )
//                         done = true;
//                 }
//             }
// /*            qDebug( "path:" + path );
//             qDebug( "ext:" + ext );
//             qDebug( "new filename:" + path + "/" + newfname + ext );*/
//             QDir dir;
//             QFile f( path + "/" + newfname + ext );
//             if ( !noall ) {
//                 if ( f.exists() ) { // File with that name already exists
//                     switch( QMessageBox::question(
//                                 this,
//                                 tr("Overwrite File? -- Media Tag Tools"),
//                                 tr("A file called %1 already exists. "
//                                 "Do you want to overwrite it?")
//                                 .arg( f.name() ),
//                                 tr("&Yes"), tr("&No"),
//                                 tr("No to &All"), 0, 1 ) ) {
//                         case 0:
//                             if ( dir.rename( ( (mttFile *) it.current() )->getFName() , path + "/" + newfname + ext ) ) {
//                                 it.current()->setText( 0, newfname + ext );
//                                 ( (mttFile *) it.current() )->setFName( path + "/" + newfname + ext );
//                             }
//                             else {
//                                 QApplication::restoreOverrideCursor();
//     
//                                 switch( QMessageBox::warning(
//                                     this,
//                                     tr("Rename File"),
//                                     tr("Rename failed for file %1!").arg( ( (mttFile *) it.current() )->getFName() ),
//                                     QMessageBox::Ok, QMessageBox::Abort, QMessageBox::NoButton ) ) {
//                                     case QMessageBox::Ok:
//                                         break;
//                                     case QMessageBox::Abort:
//                                         abort = true;
//                                 }
//     
//                                 QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//                             }
//                         case 1:
//                             break;
//                         case 2:
//                             noall = true;
//                             break;
//                     }
//                 }
//                 else
//                     if ( dir.rename( ( (mttFile *) it.current() )->getFName() , path + "/" + newfname + ext ) ) {
//                         it.current()->setText( 0, newfname + ext );
//                         ( (mttFile *) it.current() )->setFName( path + "/" + newfname + ext );
//                     }
//                     else {
//                         QApplication::restoreOverrideCursor();
// 
//                         switch( QMessageBox::warning(
//                             this,
//                             tr("Rename File"),
//                             tr("Rename failed for file %1!").arg( ( (mttFile *) it.current() )->getFName() ),
//                             QMessageBox::Ok, QMessageBox::Abort, QMessageBox::NoButton ) ) {
//                             case QMessageBox::Ok:
//                                 break;
//                             case QMessageBox::Abort:
//                                 abort = true;
//                         }
// 
//                         QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//                     }
//             }
//         }
//         ++it;
//         progress.setProgress( current++, count );
//     }
// 
//     //GenListView->clear();
//     //populateList();
// 
//     progress.hide();
//     statusBar()->message( tr( QString( "Done" ) ) );
//     QApplication::restoreOverrideCursor();
// }
// 
// void mttMainWin::slotAbout()
// {
//     mttAboutDialog about;
//     about.exec();
// }
// 
// void mttMainWin::slotCorrectCase()
// {
// }
// 
// void mttMainWin::slotAllUpper()
// {
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
// 
//     ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
//         GenTitleCLE->setText( GenTitleCLE->text().upper() );
//     }
//     if( GenArtistChkB->isChecked() ) {
//         GenArtistCLE->setText( GenArtistCLE->text().upper() );
//     }
//     if( GenAlbumChkB->isChecked() ) {
//         GenAlbumCLE->setText( GenAlbumCLE->text().upper() );
//     }
//     if( GenCommentChkB->isChecked() ) {
//         GenCommentCLE->setText( GenCommentCLE->text().upper() );
//     }
//     ignoreChange = false;
// 
//     while( it.current() ) {
//         TagLib::Tag *t;
// 
//         // Show tag info
//         t = (( mttFile *) it.current() )->getTag();
//         if ( t ) {
//             if( GenTitleChkB->isChecked() ) {
//                 t->setTitle( QStringToTString( TStringToQString( t->title() ).upper() ) );
//                 it.current()->setText( 1, TStringToQString( t->title() ) );
//             }
// 
//             if( GenArtistChkB->isChecked() ) {
//                 t->setArtist( QStringToTString( TStringToQString( t->artist() ).upper() ) );
//                 it.current()->setText( 2, TStringToQString( t->artist() ) );
//             }
// 
//             if( GenAlbumChkB->isChecked() ) {
//                 t->setAlbum( QStringToTString( TStringToQString( t->album() ).upper() ) );
//                 it.current()->setText( 3, TStringToQString( t->album() ) );
//             }
// 
//             if( GenCommentChkB->isChecked() ) {
//                 t->setComment( QStringToTString( TStringToQString( t->comment() ).upper() ) );
//                 it.current()->setText( 6, TStringToQString( t->comment() ) );
//             }
// 
//             ( (mttFile *) it.current() )->setTagChanged( true );
//         }
//         ++it;
//     }
// 
//     QApplication::restoreOverrideCursor();
// }
// 
// void mttMainWin::slotAllLower()
// {
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
// 
//     ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
//         GenTitleCLE->setText( GenTitleCLE->text().lower() );
//     }
//     if( GenArtistChkB->isChecked() ) {
//         GenArtistCLE->setText( GenArtistCLE->text().lower() );
//     }
//     if( GenAlbumChkB->isChecked() ) {
//         GenAlbumCLE->setText( GenAlbumCLE->text().lower() );
//     }
//     if( GenCommentChkB->isChecked() ) {
//         GenCommentCLE->setText( GenCommentCLE->text().lower() );
//     }
//     ignoreChange = false;
// 
//     while( it.current() ) {
//         TagLib::Tag *t;
// 
//         // Show tag info
//         t = (( mttFile *) it.current() )->getTag();
//         if ( t ) {
//             if( GenTitleChkB->isChecked() ) {
//                 t->setTitle( QStringToTString( TStringToQString( t->title() ).lower() ) );
//                 it.current()->setText( 1, TStringToQString( t->title() ) );
//             }
// 
//             if( GenArtistChkB->isChecked() ) {
//                 t->setArtist( QStringToTString( TStringToQString( t->artist() ).lower() ) );
//                 it.current()->setText( 2, TStringToQString( t->artist() ) );
//             }
// 
//             if( GenAlbumChkB->isChecked() ) {
//                 t->setAlbum( QStringToTString( TStringToQString( t->album() ).lower() ) );
//                 it.current()->setText( 3, TStringToQString( t->album() ) );
//             }
// 
//             if( GenCommentChkB->isChecked() ) {
//                 t->setComment( QStringToTString( TStringToQString( t->comment() ).lower() ) );
//                 it.current()->setText( 6, TStringToQString( t->comment() ) );
//             }
// 
//             ( (mttFile *) it.current() )->setTagChanged( true );
//         }
//         ++it;
//     }
// 
//     QApplication::restoreOverrideCursor();
// }
// 
// void mttMainWin::slotFirstUpWords()
// {
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
//         GenTitleCLE->setText( firstUp( GenTitleCLE->text().lower() ) );
//     }
//     if( GenArtistChkB->isChecked() ) {
//         GenArtistCLE->setText( firstUp( GenArtistCLE->text().lower() ) );
//     }
//     if( GenAlbumChkB->isChecked() ) {
//         GenAlbumCLE->setText( firstUp( GenAlbumCLE->text().lower() ) );
//     }
//     if( GenCommentChkB->isChecked() ) {
//         GenCommentCLE->setText( firstUp( GenCommentCLE->text().lower() ) );
//     }
//     ignoreChange = false;
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//     while( it.current() ) {
//         TagLib::Tag *t;
// 
//         // Show tag info
//         t = (( mttFile *) it.current() )->getTag();
//         if ( t ) {
//             if( GenTitleChkB->isChecked() ) {
//                 t->setTitle( QStringToTString( firstUp( TStringToQString( t->title() ).lower() ) ) );
//                 it.current()->setText( 1, TStringToQString( t->title() ) );
//             }
// 
//             if( GenArtistChkB->isChecked() ) {
//                 t->setArtist( QStringToTString( firstUp( TStringToQString( t->artist() ).lower() ) ) );
//                 it.current()->setText( 2, TStringToQString( t->artist() ) );
//             }
// 
//             if( GenAlbumChkB->isChecked() ) {
//                 t->setAlbum( QStringToTString( firstUp( TStringToQString( t->album() ).lower() ) ) );
//                 it.current()->setText( 3, TStringToQString( t->album() ) );
//             }
// 
//             if( GenCommentChkB->isChecked() ) {
//                 t->setComment( QStringToTString( firstUp( TStringToQString( t->comment() ).lower() ) ) );
//                 it.current()->setText( 6, TStringToQString( t->comment() ) );
//             }
// 
//             ( (mttFile *) it.current() )->setTagChanged( true );
//         }
//         ++it;
//     }
// 
//     QApplication::restoreOverrideCursor();
// }
// 
// void mttMainWin::slotFirstUpSentence() // Here is the change to get only the first letter uppercase (only track name)
// {
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
//         GenTitleCLE->setText( firstUpSentence( GenTitleCLE->text().lower() ) );
//     }
//     if( GenArtistChkB->isChecked() ) {
//         GenArtistCLE->setText( firstUpSentence( GenArtistCLE->text().lower() ) );
//     }
//     if( GenAlbumChkB->isChecked() ) {
//         GenAlbumCLE->setText( firstUpSentence( GenAlbumCLE->text().lower() ) );
//     }
//     if( GenCommentChkB->isChecked() ) {
//         GenCommentCLE->setText( firstUpSentence( GenCommentCLE->text().lower() ) );
//     }
//     ignoreChange = false;
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//     while( it.current() ) {
//         TagLib::Tag *t;
// 
//         // Show tag info
//         t = (( mttFile *) it.current() )->getTag();
//         if ( t ) {
//             if( GenTitleChkB->isChecked() ) {
//                 t->setTitle( QStringToTString( firstUpSentence( TStringToQString( t->title() ).lower() ) ) );
//                 it.current()->setText( 1, TStringToQString( t->title() ) );
//             }
// 
//             if( GenArtistChkB->isChecked() ) {
//                 t->setArtist( QStringToTString( firstUpSentence( TStringToQString( t->artist() ).lower() ) ) );
//                 it.current()->setText( 2, TStringToQString( t->artist() ) );
//             }
// 
//             if( GenAlbumChkB->isChecked() ) {
//                 t->setAlbum( QStringToTString( firstUpSentence( TStringToQString( t->album() ).lower() ) ) );
//                 it.current()->setText( 3, TStringToQString( t->album() ) );
//             }
// 
//             if( GenCommentChkB->isChecked() ) {
//                 t->setComment( QStringToTString( firstUpSentence( TStringToQString( t->comment() ).lower() ) ) );
//                 it.current()->setText( 6, TStringToQString( t->comment() ) );
//             }
// 
//             ( (mttFile *) it.current() )->setTagChanged( true );
//         }
//         ++it;
//     }
// 
//     QApplication::restoreOverrideCursor();
// }
// 
// 
// void mttMainWin::slotEmptyFields()
// {
//     GenTitleCLE->clear();
//     GenArtistCLE->clear();
//     GenAlbumCLE->clear();
//     GenYearCLE->clear();
//     GenGenreCB->clearEdit();
//     GenCommentCLE->clear();
//     GenTrackCLE->clear();
// }
// 
// QString mttMainWin::firstUpSentence( QString str )
// {
//     if ( str[0].isLetter() ) {
//         str[0] = str[0].upper();
//     }
// 
//     return str;
// }
// 
// QString mttMainWin::firstUp( QString str )
// {
//     unsigned int i;
// 
//     i = 0;
//     while ( i < str.length() ) {
//         if ( i == 0 && str[i].isLetter() ) {
//             str[i] = str[i].upper();
//         }
//         if ( str[i].isSpace() && ( ( i + 1 ) < str.length() ) && str[i+1].isLetter() ) {
//             str[i+1] = str[i+1].upper();
//         }
//         i++;
//     }
// 
//     return str;
// }
// 
// void mttMainWin::slotLVRightMenu()
// {
//     Q3PopupMenu menu, corCaseMenu;
// 
//     corCaseMenu.insertItem( tr( "First letter up (first word)" ), this, SLOT(slotFirstUpSentence()) );
//     corCaseMenu.insertItem( tr( "First letter up (each word)" ), this, SLOT(slotFirstUpWords()) );
//     corCaseMenu.insertItem( tr( "All uppercase" ), this, SLOT(slotAllUpper()) );
//     corCaseMenu.insertItem( tr( "All lowercase" ), this, SLOT(slotAllLower()) );
// 
//     menu.insertItem( tr( "Open folder" ), this, SLOT(slotOpen()) );
//     menu.insertItem( tr( "Add file(s)" ), this, SLOT(slotOpenFiles()) );
//     menu.insertItem( tr( "Remove file(s)" ), this, SLOT(slotRemoveFiles()) );
//     menu.insertSeparator();
//     menu.insertItem( tr( "Write tags" ), this, SLOT(slotSaveTags()) );
//     menu.insertItem( tr( "Write selected tags only" ), this, SLOT(slotSaveSelectedTags()) );
//     menu.insertItem( tr( "Remove tag" ), this, SLOT(slotRemoveTags()) );
//     menu.insertSeparator();
//     menu.insertItem( tr( "Correct Case" ), &corCaseMenu );
//     menu.insertItem( tr( "Fix tag (iso->utf8)" ), this, SLOT(slotFixTags()) );
//     menu.exec( QCursor::pos() );
// }
// 
// void mttMainWin::slotFixTags()
// {
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     while ( it.current() ) {
//         if ( ( (mttFile *) it.current() )->isMpeg() ) {
//             TagLib::ID3v2::Tag *tag = ( (mttFile *) it.current() )->getID3Tag();
//             if ( tag ) {
//                 TagLib::ID3v2::FrameList l = tag->frameList();
//                 TagLib::List<TagLib::ID3v2::Frame *>::Iterator fit;
//     
//                 for ( fit=l.begin(); fit != l.end(); fit++ ) {
//                     if ( *( (*fit)->frameID().data() ) == 'T' ) {
//                         TagLib::ID3v2::TextIdentificationFrame *f = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *> (*fit);
//     
//                         if ( f ) {
//                             qDebug( QString::number( f->textEncoding() ) );
//                             qDebug( f->toString().toCString( false ) );
//                             TagLib::String fixed( f->toString().toCString(), TagLib::String::UTF8 );
//                             if ( f->textEncoding() == TagLib::String::Latin1 ) { // latin1
//                                 f->setTextEncoding( TagLib::String::UTF8 );
//                                 f->setText( fixed );
//                             }
//                         }
//                         else
//                         qDebug( "fix = NULL" );
//                     } // end if frameID starts with T
//                 } // for each text frame
//                 ( (mttFile *) it.current() )->saveTag();
//     
//                 TagLib::Tag *t;
//     
//                 // Show tag info
//                 t = (( mttFile *) it.current() )->getTag();
//                 if ( t ) {
//                     it.current()->setText( 1, TStringToQString( t->title() ) );
//                     it.current()->setText( 2, TStringToQString( t->artist() ) );
//                     it.current()->setText( 3, TStringToQString( t->album() ) );
//                     it.current()->setText( 4, QString::number( t->year() ) );
//                     it.current()->setText( 5, TStringToQString( t->genre() ) );
//                     it.current()->setText( 6, TStringToQString( t->comment() ) );
//                     it.current()->setText( 7, QString::number( t->track() ) );
//                 }
//                 // TODO: Show media info
//             }
//         } // end if is Mpeg file
//         ++it;
//     } // for each selected item
//     //GenListView->clear();
//     //populateList();
// }
// 
// void mttMainWin::slotTitleEnter()
// {
//     if ( GenArtistCLE->isEnabled() )
//         GenArtistCLE->setFocus();
//     else if ( GenAlbumCLE->isEnabled() )
//         GenAlbumCLE->setFocus();
//     else if ( GenYearCLE->isEnabled() )
//         GenYearCLE->setFocus();
// //    else if ( GenGenreCB->isEnabled() )
// //        GenGenreCB->setFocus();
//     else if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotArtistEnter()
// {
//     if ( GenAlbumCLE->isEnabled() )
//         GenAlbumCLE->setFocus();
//     else if ( GenYearCLE->isEnabled() )
//         GenYearCLE->setFocus();
// /*    else if ( GenGenreCB->isEnabled() )
//         GenGenreCB->setFocus();*/
//     else if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotAlbumEnter()
// {
//     if ( GenYearCLE->isEnabled() )
//         GenYearCLE->setFocus();
// /*    else if ( GenGenreCB->isEnabled() )
//         GenGenreCB->setFocus();*/
//     else if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotYearEnter()
// {
// /*    if ( GenGenreCB->isEnabled() )
//         GenGenreCB->setFocus();*/
//     if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotGenreEnter()
// {
//     if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotCommentEnter()
// {
//     if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }
// 
// void mttMainWin::slotNextEntry()
// {
//     Q3ListViewItem *c;
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     c = it.current();
//     if ( c ) {
//         if ( c->itemBelow() ) {
//             GenListView->clearSelection();
//             GenListView->setSelected( c->itemBelow(), TRUE );
//             GenListView->ensureItemVisible( c->itemBelow() );
//         }
//     }
//     else
//         GenListView->setSelected( GenListView->firstChild(), TRUE );
// }
// 
// void mttMainWin::slotPreviousEntry()
// {
//     Q3ListViewItem *c;
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     c = it.current();
//     if ( c ) {
//         if ( c->itemAbove() ) {
//             GenListView->clearSelection();
//             GenListView->setSelected( c->itemAbove(), TRUE );
//             GenListView->ensureItemVisible( c->itemAbove() );
//         }
//     }
//     else
//         GenListView->setSelected( GenListView->firstChild(), TRUE );
// }
// 
// void mttMainWin::slotPreviousPage()
// {
// }
// 
// void mttMainWin::slotNextPage()
// {
// }
// 
// void mttMainWin::slotTitleChanged( const QString &title )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( TStringToQString( t->title() ) != title ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setTitle( QStringToTString( title ) );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 1, title );
//             }
//         it++;
//         }
//     }
// }
// 
// void mttMainWin::slotSelectionChange()
// {
//     bool updateSelectedFname = false;
// 
// //     qDebug( "Selection Change" );
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
// 
//     if ( !selectedFname.isEmpty() ) { // If there is an item selected
//         Q3ListViewItem *lvi = GenListView->findItem( selectedFname, 0 );
//         if ( lvi && !lvi->isSelected() )
//             updateSelectedFname = true;
//     }
//     if ( selectedFname.isEmpty() || updateSelectedFname || ( GenListView->findItem( selectedFname, 0 ) == NULL ) ) {
// 
//         // Set the contents of the widgets according to the tag
//         TagLib::Tag *t;
// 
//         if ( it.current() ) {
//             // Show tag info
//             t = (( mttFile *) it.current() )->getTag();
//             if ( t ) {
//                 GenTitleCLE->setText( TStringToQString( t->title() ) );
//                 GenArtistCLE->setText( TStringToQString( t->artist() ) );
//                 GenAlbumCLE->setText( TStringToQString( t->album() ) );
//                 GenYearCLE->setText( QString::number( t->year() ) );
//                 GenGenreCB->setCurrentText( TStringToQString( t->genre() ) );
//                 GenCommentCLE->setText( TStringToQString( t->comment() ) );
//                 GenTrackCLE->setText( QString::number( t->track() ) );
// 
//                 updateAdvMp3TagTable( ( ( mttFile *) it.current() )->getMp3ExtraFrames() );
//             }
//             else {
//                 GenTitleCLE->setText( "" );
//                 GenArtistCLE->setText( "" );
//                 GenAlbumCLE->setText( "" );
//                 GenYearCLE->setText( "" );
//                 GenGenreCB->setCurrentText( "" );
//                 GenCommentCLE->setText( "" );
//                 GenTrackCLE->setText( "" );
//             }
//             selectedFname = (( mttFile *) it.current() )->getFName();
//             selectedFname = selectedFname.mid( selectedFname.findRev( "/" ) + 1 );
//         }
//     }
// }
// 
// void mttMainWin::slotTrackChanged( const QString &track )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if (  t->track() != track.toUInt() ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setTrack( track.toUInt() );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 7, track );
//             }
//         it++;
//         }
//     }
// }
// 
// 
// void mttMainWin::slotCommentChanged( const QString &comment )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( TStringToQString( t->comment() ) != comment ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setComment( QStringToTString( comment ) );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 6, comment );
//             }
//         it++;
//         }
//     }
// }
// 
// 
// void mttMainWin::slotYearChanged( const QString &year )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( t->year() != year.toUInt() ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setYear( year.toUInt() );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 4, year );
//             }
//         it++;
//         }
//     }
// }
// 
// 
// void mttMainWin::slotAlbumChanged( const QString &album )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( TStringToQString( t->album() ) != album ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setAlbum( QStringToTString( album ) );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 3, album );
//             }
//         it++;
//         }
//     }
// }
// 
// 
// void mttMainWin::slotArtistChanged( const QString &artist )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( TStringToQString( t->artist() ) != artist ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setArtist( QStringToTString( artist ) );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 2, artist );
//             }
//         it++;
//         }
//     }
// }
// 
// void mttMainWin::slotGenreChanged( const QString &genre )
// {
//     TagLib::Tag *t;
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     if ( !ignoreChange ) {
//         while ( it.current() ) {
//             t = ( (mttFile *) it.current() )->getTag( true );
//             if ( TStringToQString( t->genre() ) != genre ) { // A bit of a double check but just to be sure...
//                 ( (mttFile *) it.current() )->checkEncodings();
//                 // Save info from the various text fields
//                 t->setGenre( QStringToTString( genre ) );
//                 ( (mttFile *) it.current() )->setTagChanged( true );
//                 it.current()->setText( 5, genre );
//             }
//         it++;
//         }
//     }
// }
// 
// void mttMainWin::saveTags( bool selectedOnly )
// {
//     int current, count;
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//     statusBar()->message( tr( QString( "Writing tags..." ) ) );
//     count = GenListView->childCount();
//     current = 1;
//     progress.show();
//     progress.setProgress( 0, count );
//     TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);
//     Q3ListViewItemIterator it;
//     if ( selectedOnly ) {
//         Q3ListViewItemIterator tmp( GenListView, Q3ListViewItemIterator::Selected );
//         it = tmp;
//         }
//     else {
//         Q3ListViewItemIterator tmp( GenListView );
//         it = tmp;
//         }
//     while ( it.current() ) {
//         if ( ( (mttFile *) it.current() )->tagChanged() ) {
//             ( (mttFile *) it.current() )->checkEncodings();
// 
//             ( (mttFile *) it.current() )->saveTag();
//             ( (mttFile *) it.current() )->setTagChanged( false );
// 
//             // Update the ListView too
//             ( (mttFile *) it.current() )->repaint();
//         }
//         ++it;
// 
//         progress.setProgress( current++, count );
//     }
// 
//     QApplication::restoreOverrideCursor();
//     progress.hide();
//     statusBar()->message( tr( QString( "Done" ) ) );
// }
// 
// void mttMainWin::slotCreateTags()
// {
//     TagLib::Tag *t;
//     int current, count;
// 
//     QApplication::setOverrideCursor( QCursor( Qt::waitCursor ) );
//     statusBar()->message( tr( QString( "Writing tags..." ) ) );
//     count = GenListView->childCount();
//     current = 1;
//     progress.show();
//     progress.setProgress( 0, count );
//     TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);
//     Q3ListViewItemIterator it;
//     Q3ListViewItemIterator tmp( GenListView, Q3ListViewItemIterator::Selected );
//     it = tmp;
//     while ( it.current() ) {
//         t = ( (mttFile *) it.current() )->getTag( true );
//         ( (mttFile *) it.current() )->checkEncodings();
// 
//         // Save info using the filename and the custom format string
//         if ( UseCFChkB->isChecked() && ( MCFormatLE->text() != "" ) ) { // Custom format is enabled
//             QString formatstr = MCFormatLE->text();
//             QStringList::Iterator slit = separators.begin();
//             QString filename = ( (mttFile *) it.current() )->getFName();
//             int curpos, pos1, pos2;
//             bool done;
//     
//             // Remove path from filename
//             curpos = filename.findRev( "/" );
//             if ( curpos != -1 )
//                 filename.remove( 0, curpos + 1 );
//             // Remove extension too
//             curpos = filename.findRev( "." );
//             if ( curpos != -1 )
//                 filename.truncate( curpos );
//     
//             done = false;
//             while ( !done ) {
//                 pos1 = formatstr.find( ">" );
//                 pos2 = formatstr.find( "<", 1 );
//                 if ( pos1 == -1 ) // No more <> tags
//                     done = true;
//                 else if ( pos2 == -1 ) { // Last <> tag
//                     if ( formatstr.startsWith( QString( "<artist>" ) ) )
//                         t->setArtist( QStringToTString( filename ) );
//                     if ( formatstr.startsWith( QString( "<album>" ) ) )
//                         t->setAlbum( QStringToTString( filename ) );
//                     if ( formatstr.startsWith( QString( "<year>" ) ) )
//                         t->setYear( filename.toInt() );
//                     if ( formatstr.startsWith( QString( "<title>" ) ) )
//                         t->setTitle( QStringToTString( filename ) );
//                     if ( formatstr.startsWith( QString( "<comment>" ) ) )
//                         t->setComment( QStringToTString( filename ) );
//                     if ( formatstr.startsWith( QString( "<genre>" ) ) )
//                         t->setGenre( QStringToTString( filename ) );
//                     if ( formatstr.startsWith( QString( "<track>" ) ) )
//                         t->setTrack( filename.toInt() );
//                     done = true;
//                 }
//                 else { // <> tag & separator exist
//                     int pos = filename.find( formatstr.mid( pos1 + 1, pos2 - pos1 -1 ) );
//                     if ( pos != -1 ) {
//                         QString data = filename.left( pos );
// 
//                         if ( formatstr.startsWith( QString( "<artist>" ) ) )
//                             t->setArtist( QStringToTString( data ) );
//                         if ( formatstr.startsWith( QString( "<album>" ) ) )
//                             t->setAlbum( QStringToTString( data ) );
//                         if ( formatstr.startsWith( QString( "<year>" ) ) )
//                             t->setYear( data.toInt() );
//                         if ( formatstr.startsWith( QString( "<title>" ) ) )
//                             t->setTitle( QStringToTString( data ) );
//                         if ( formatstr.startsWith( QString( "<comment>" ) ) )
//                             t->setComment( QStringToTString( data ) );
//                         if ( formatstr.startsWith( QString( "<genre>" ) ) )
//                             t->setGenre( QStringToTString( data ) );
//                         if ( formatstr.startsWith( QString( "<track>" ) ) )
//                             t->setTrack( data.toInt() );
// 
//                         //qDebug( "filename:" + filename );
//                         //qDebug( "formatstr:" + formatstr );
//                         //qDebug( "data:" + data );
//                         //qDebug( "mid:" + formatstr.mid( pos1 + 1, pos2 - pos1 - 1 ) );
//                         filename.remove( 0, pos + pos2 - pos1 - 1 );
//                         formatstr.remove( 0, pos2 );
//                         //qDebug( "filename:" + filename );
//                         //qDebug( "formatstr:" + formatstr );
//                     }
//                     else
//                          done = true;
//                 }
//             }
//  
//             ( (mttFile *) it.current() )->setTagChanged( true );
// 
//             // Update the ListView too
//             ( (mttFile *) it.current() )->repaint();
//             ( (mttFile *) it.current() )->setText( 1, TStringToQString( t->title() ) );
//             ( (mttFile *) it.current() )->setText( 2, TStringToQString( t->artist() ) );
//             ( (mttFile *) it.current() )->setText( 3, TStringToQString( t->album() ) );
//             ( (mttFile *) it.current() )->setText( 4, QString::number( t->year() ) );
//             ( (mttFile *) it.current() )->setText( 5, TStringToQString( t->genre() ) );
//             ( (mttFile *) it.current() )->setText( 6, TStringToQString( t->comment() ) );
//             ( (mttFile *) it.current() )->setText( 7, QString::number( t->track() ) );
// 
//             // TODO: Use path & filename instead of just the name for the check
//             filename = ( (mttFile *) it.current() )->getFName();
//             curpos = filename.findRev( "/" );
//             if ( curpos != -1 )
//                 filename.remove( 0, curpos + 1 );
// 
//             // Update current file info
//             ignoreChange = true; // Ignore the following change to the line edits
//             if ( filename ==  selectedFname ) {
//                 GenTitleCLE->setText( TStringToQString( t->title() ) );
//                 GenArtistCLE->setText( TStringToQString( t->artist() ) );
//                 GenAlbumCLE->setText( TStringToQString( t->album() ) );
//                 GenYearCLE->setText( QString::number( t->year() ) );
//                 GenGenreCB->setCurrentText( TStringToQString( t->genre() ) );
//                 GenCommentCLE->setText( TStringToQString( t->comment() ) );
//                 GenTrackCLE->setText( QString::number( t->track() ) );
//             }
//             ignoreChange = false;
//         }
//         ++it;
// 
//         progress.setProgress( current++, count );
//     }
// 
//     QApplication::restoreOverrideCursor();
//     progress.hide();
//     statusBar()->message( tr( QString( "Done" ) ) );
// }
// 
// void mttMainWin::slotAdvTagValueChanged( int row, int column )
// {
// //     qDebug( QString::number( row ) + "-" + QString::number( column ) );
// 
//     AdvTagTable->setEnabled( false );
// 
//     if ( column == field_col ) {
//         if ( ( row == ( AdvTagTable->numRows() - 1 ) ) && ( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentItem() != 0 ) ) { // If the last item of the table has changed and it is anything but <Empty> ...
// //             qDebug( QString( "Add" ) + ( ( QComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() );
//             int i;
// 
//             for ( i = 0; i < AdvTagTable->numRows(); i++ ) { // Remove the same option from every other combobox
//                 if ( i != row ) {
//                     QStringList l;
//                     QString current;
//                     int j;
// 
//                     current = ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->currentText();
//                     for ( j = 0; j < ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->count(); j++ ) {
//                         if ( ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j ) != ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() )
//                             l += ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j );
//                     }
// 
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setStringList( l );
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setCurrentItem( current );
//                 }
//             }
// 
//             availExtraFrames.remove( availExtraFrames.find( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() ) );
//             xtraFrames += ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText();
// 
//             // Find FrameID
//             QString fid;
//             for ( i = 0; i < EF_NUM; i++ ) {
//                 if ( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() == extraFrames[i][1] ) {
//                     fid = extraFrames[i][0];
//                 }
//             }
// //             qDebug( fid + "--" );
//             AdvTagTable->setText( row, field_id_col, fid );
//             AdvTagTable->insertRows( AdvTagTable->numRows() );
//             AdvTagTable->setItem( AdvTagTable->numRows() - 1, field_col, new Q3ComboTableItem( AdvTagTable, availExtraFrames ) );
//             QPixmap pix( ( const char ** ) x_xpm );
//             AdvTagTable->setPixmap( AdvTagTable->numRows() - 1, x_col, pix );
// 
//         }
//         else if ( ( row != ( AdvTagTable->numRows() - 1 ) ) && ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentItem() == 0 ) { // If any other item than the last item of the list has changed and it is now <Empty>
// //             qDebug( "Del" );
//             AdvTagTable->removeRow( row );
//             availExtraFrames += *xtraFrames.at( row );
//             availExtraFrames.sort();
// 
//             int i;
// 
//             for ( i = 0; i < AdvTagTable->numRows(); i++ ) { // Add the same option in every other combobox
//                 QStringList l;
//                 QString current;
//                 int j;
// 
//                 current = ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->currentText();
//                 for ( j = 0; j < ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->count(); j++ )
//                     l += ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j );
// 
//                 l += *xtraFrames.at( row );
//                 l.sort();
//                 ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setStringList( l );
//                 ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setCurrentItem( current );
//             }
//             xtraFrames.remove( xtraFrames.at( row ) );
//         }
//         else if ( ( row != ( AdvTagTable->numRows() - 1 ) ) && ( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentItem() != 0 ) ) { // If any other item except the last has changed to something else than <Empty>
//             availExtraFrames += *xtraFrames.at( row );
//             availExtraFrames.sort();
// 
//             int i;
// 
//             for ( i = 0; i < AdvTagTable->numRows(); i++ ) { // Add the same option in every other combobox
//                 if ( i != row ) {
//                     QStringList l;
//                     QString current;
//                     int j;
// 
//                     current = ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->currentText();
//                     for ( j = 0; j < ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->count(); j++ )
//                         l += ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j );
// 
//                     l += *xtraFrames.at( row );
//                     l.sort();
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setStringList( l );
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setCurrentItem( current );
//                 }
//             }
//             xtraFrames.remove( xtraFrames.at( row ) );
// 
//             for ( i = 0; i < AdvTagTable->numRows(); i++ ) { // Remove the same option from every other combobox
//                 if ( i != row ) {
//                     QStringList l;
//                     QString current;
//                     int j;
// 
//                     current = ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->currentText();
//                     for ( j = 0; j < ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->count(); j++ ) {
//                         if ( ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j ) != ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() )
//                             l += ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->text( j );
//                     }
// 
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setStringList( l );
//                     ( ( Q3ComboTableItem * ) AdvTagTable->item( i, column ) )->setCurrentItem( current );
//                 }
//             }
// 
//             availExtraFrames.remove( availExtraFrames.find( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() ) );
//             xtraFrames += ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText();
// 
//             // Find FrameID
//             QString fid;
//             for ( i = 0; i < EF_NUM; i++ ) {
//                 if ( ( ( Q3ComboTableItem * ) AdvTagTable->item( row, column ) )->currentText() == extraFrames[i][1] ) {
//                     fid = extraFrames[i][0];
//                 }
//             }
// //             qDebug( fid + "--" );
//             AdvTagTable->setText( row, field_id_col, fid );
//         }
//     }
//     else if ( column == value_col ) {
//         AdvTagTable->adjustColumn( column );
//     }
// 
//     if ( !ignoreChange ) {
//         // Create a list of extra frames for the selected files
//         QStringList eframes;
//         int i;
// 
//         for ( i = 0; i < ( AdvTagTable->numRows() - 1 ); i++ ) {
//             if ( ( ( Q3ComboTableItem * ) AdvTagTable->item( i, field_col ) )->currentItem() != 0 ) {
//                 eframes += AdvTagTable->text( i, field_id_col );
//                 eframes += AdvTagTable->text( i, value_col );
//             }
//         }
// 
// 
//         Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//         while ( it.current() ) {
//             ( (mttFile *) it.current() )->setMp3ExtraFrames( eframes );
//             ( (mttFile *) it.current() )->setTagChanged( true );
//             ( (mttFile *) it.current() )->repaint();
//             ++it;
//         }
//     }
// 
//     AdvTagTable->setEnabled( true );
//     AdvTagTable->setCurrentCell( 0, field_id_col ); // Possible QT bug? You need to do this otherwise QComboTableItem seems to not work some times
// }
// 
// void mttMainWin::updateAdvMp3TagTable( QStringList strl ) {
// 
//     ignoreChange = true;
// 
//     // Clear the table
//     slotRemoveAdvTags();
// 
//     // Fill the table with the extra tags
//     int row = 0, i;
// 
// //     qDebug( "\n\netags = \n" );
//     for ( QStringList::Iterator it = strl.begin(); it != strl.end(); ++it ) {
// //         qDebug( QString( "\"" ) + *it + "\"=" );
//         for ( i = 0; i < EF_NUM; i++ ) {
//             if ( *it == extraFrames[i][0] ) {
//                 ( ( Q3ComboTableItem * ) AdvTagTable->item( row, field_col ) )->setCurrentItem( extraFrames[i][1] );
//                 slotAdvTagValueChanged( row, field_col );
//                 ++it;
// //                 qDebug( *it + "\n" );
//                 AdvTagTable->setText( row, value_col, *it );
//                 row++;
//             }
//         }
//     }
// 
//     AdvTagTable->adjustColumn( value_col );
// 
//     ignoreChange = false;
// }
// 
// void mttMainWin::slotRemoveAdvTags()
// {
//     // Clear the table
//     int i, num = AdvTagTable->numRows() - 1;
// 
//     for( i = 0; i < num; i++ ){
//         ( ( Q3ComboTableItem * ) AdvTagTable->item( 0, field_col ) )->setCurrentItem( 0 );
//         slotAdvTagValueChanged( 0, field_col );
//     }
// }
// 
// void mttMainWin::slotRemoveFiles()
// {
//     Q3PtrList<Q3ListViewItem> list;
//     list.setAutoDelete( TRUE ); // the list owns the objects
// 
//     Q3ListViewItemIterator it( GenListView, Q3ListViewItemIterator::Selected );
//     while ( it.current() ) {
//         list.append( *it );
//         ++it;
//     }
// 
//     list.clear(); // Delete list items
// }
// 
// void mttMainWin::slotDroppedUris( QStringList qsl )
// {
//     for ( QStringList::Iterator it = qsl.begin(); it != qsl.end(); ++it ) {
//         //qDebug( *it );
// 
//         Q3Url url( *it );
// 
//         if ( url.isLocalFile() ) {
// //             qDebug( url.path() );
//             QFileInfo fi( url.path() );
// 
//             if ( fi.exists() ) {
//                 if ( fi.isDir() )
//                     addDir( url.path() );
//                 else {
//                     if ( url.fileName().endsWith( ".mp3", FALSE ) ||
//                          url.fileName().endsWith( ".ogg", FALSE ) ||
//                          url.fileName().endsWith( ".flac", FALSE ) ) {
//                         curPath = url.dirPath();
//                         addFile( url.path() );
//                     }
//                 }
//             }
//         }
//     }
// }
// 
// void mttMainWin::slotXButtonClickedMP3( int row, int col )
// {
//     if ( col == x_col ) { // If an X was clicked...
//         if ( ( AdvTagTable->numRows() != 1 ) && ( row != ( AdvTagTable->numRows() - 1 ) ) ) { // If we have more than one advanced tag and we haven't clicked on the X on the last row of the table...
//             ( ( Q3ComboTableItem * ) AdvTagTable->item( row, field_col ) )->setCurrentItem( 0 );
//             slotAdvTagValueChanged( row, field_col );
//         }
//     }
// }
// 
