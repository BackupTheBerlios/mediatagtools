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

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qtabbar.h>
#include <qtabwidget.h>

#include <tag.h>
#include <tlist.h>
#include <tstring.h>
#include <id3v2framefactory.h>
#include <id3v2tag.h>
#include <textidentificationframe.h>

#include "qclineedit.h"
#include "mttmainwin.h"
#include "alistviewitem.h"
#include "mttcfdialog.h"
#include "mttaboutdialog.h"
#include "config.h"
#ifndef RELEASE
#include "revision.h"
#endif


mttMainWin::mttMainWin(QWidget* parent, const char* name, WFlags fl)
: MainForm(parent,name,fl)
{
    int i;

    LZ = true; // Leading Zeros
    LZ1 = "0";
    LZ2 = "";

    d.setFilter( QDir::Files | QDir::Readable );
    //d.setNameFilter( QString( "*.mp3" ) );
    d.setNameFilter( QString( "*.mp3;*.ogg;*.flac" ) );

    for ( i=0; i<5; i++ ) {
        separators << " - ";
    }
#ifndef RELEASE
    setCaption( caption() + " - " + RV_SNAPSHOT_VERSION );
#endif

    tabWidget->removePage( tabWidget->page( 1 ) );
    tabWidget->removePage( tabWidget->page( 1 ) );
    UseDFChkBox->hide();
    comboBox1->hide();
    CleanFButton->hide();
    CreateDirButton->hide();

    progress.setPercentageVisible( true );
    progress.setCenterIndicator( true );
    progress.setMaximumWidth( 100 );
    progress.setMaximumHeight( 20 );
    progress.setProgress( 100, 100 );
    statusBar()->addWidget( &progress, 0, true );
    statusBar()->message( tr( QString( "Ready" ) ) );
    progress.hide();
}

mttMainWin::~mttMainWin()
{
}

void mttMainWin::slotOpen()
{
    QFileDialog fd;
    bool done = false;

    fd.setMode( QFileDialog::Directory );
    //fd.addFilter( "Mp3 Files (*.mp3)" );
    fd.addFilter( tr( "Audio Files (*.mp3 *.ogg *.flac)" ) );
    fd.setDir( d.path() );
    while (!done) {
        if ( fd.exec() == QDialog::Accepted ) {
            //qDebug( fd.selectedFile().utf8() );
            d.setPath( QString( fd.selectedFile() ) );
            if ( d.exists() )
                done = true;
            else
                QMessageBox::critical( this, tr( "Error" ), tr( "Dir "+QString( fd.selectedFile() )+" not found!" ), QMessageBox::Ok, QMessageBox::NoButton );
        }
        else
            return;
    }

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    GenListView->clear();
    populateList();
    QApplication::restoreOverrideCursor();
    selectedFname = "";
}

void mttMainWin::slotOpenFiles()
{
    QFileDialog fd;
    QStringList files;
    QString dirpath;
    AListViewItem *li;
    TagLib::Tag *t;
    int count = 0, current = 1;

    fd.setMode( QFileDialog::ExistingFiles );
    //fd.addFilter( "Mp3 Files (*.mp3)" );
    fd.addFilter( tr( "Audio Files (*.mp3 *.ogg *.flac)" ) );
    fd.setDir( d.path() );
    if ( fd.exec() == QDialog::Accepted ) {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        statusBar()->message( tr( QString( "Reading tags..." ) ) );

        files = fd.selectedFiles();
        dirpath = fd.dirPath();

        for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
        count++;
        }

        progress.show();
        progress.setProgress( 0, count );

        for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it ) {
            qDebug( *it );
            li = new AListViewItem( GenListView );
            li->setText( 0, (*it).right( (*it).length() - dirpath.length() - 1 ) );
            //qDebug( QString( (*it).right( (*it).length() - dirpath.length() - 1 ).utf8() ) );
            /*if ( QFile::exists( d.path() + "/" + *it ) )
                qDebug( "exists" );
            else
                qDebug( "doesn't exist" );*/
            li->FileRef( *it );
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
                qDebug( "tag = NULL" );
            progress.setProgress( current++, count );
        }
        progress.hide();
        statusBar()->message( tr( QString( "Done" ) ) );
        QApplication::restoreOverrideCursor();
    }
    else
        return;

    selectedFname = "";
}

void mttMainWin::populateList( void )
{
    QStringList fnames;
    AListViewItem *li;
    TagLib::Tag *t;
    int current, count;

    fnames = d.entryList();
    count = d.count();
    current = 1;
    progress.show();
    progress.setProgress( 0, count );
    statusBar()->message( tr( QString( "Reading tags..." ) ) );

    for ( QStringList::Iterator it = fnames.begin(); it != fnames.end(); ++it ) {
        li = new AListViewItem( GenListView );
        li->setText( 0, *it );
        //qDebug( QString( d.path() + "/" + *it ).utf8() );
        /*if ( QFile::exists( d.path() + "/" + *it ) )
            qDebug( "exists" );
        else
            qDebug( "doesn't exist" );*/
        li->FileRef( QString( d.path() + "/" + *it ) );
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
            qDebug( "tag = NULL" );
        progress.setProgress( current++, count );
    }

    progress.hide();
    statusBar()->message( tr( QString( "Done" ) ) );
}

void mttMainWin::slotSaveTags()
{
    saveTags( false );
}

void mttMainWin::slotSaveSelectedTags()
{
    saveTags( true );
}

void mttMainWin::slotRemoveTags()
{
    int count, current, button;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    if ( it.current() ) {
        button = QMessageBox::question( this, tr( "Remove tags? -- Media Tag Tools" ), tr( "Are you sure you want to completely remove the tag(s)?" ), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape );

        if ( button == QMessageBox::Yes ) {
            QApplication::setOverrideCursor( QCursor( Qt::busyCursor ) );
            statusBar()->message( tr( QString( "Removing tags..." ) ) );
            count = GenListView->childCount();
            current = 1;
            progress.show();
            progress.setProgress( 0, count );
            while ( it.current() ) {
                ( (AListViewItem *) it.current() )->removeTag();
                ++it;
                progress.setProgress( current++, count );
            }
    
            GenListView->clear();
            populateList();
            progress.hide();
            statusBar()->message( tr( QString( "Done" ) ) );
            QApplication::restoreOverrideCursor();
        }
    }
}

void mttMainWin::slotCFormat()
{
    mttCFDialog cfdialog;

    cfdialog.setFormat( MCFormatLE->text() );
    cfdialog.setSeparators( separators );
    cfdialog.setLZ1( LZ1 );
    cfdialog.setLZ2( LZ2 );
    cfdialog.enableLZ( LZ );
    if ( cfdialog.exec() == QDialog::Accepted ) {
        MCFormatLE->setText( cfdialog.getFormat() );
        separators = cfdialog.getSeparators();
        LZ1 = cfdialog.getLZ1();
        LZ2 = cfdialog.getLZ2();
        LZ = cfdialog.isLZOn();
    }
    if ( UseCFChkB->isChecked() ) {
        slotDisableUsingFormat( false ); // Reset state of fields
        slotDisableUsingFormat( true ); // and set new state according to format string
    }
}

void mttMainWin::slotDisableUsingFormat( bool cond )
{
    if ( cond ) {
        QString formatstr = MCFormatLE->text();

        if ( formatstr.find( "<artist>" ) != -1 ) {
            GenArtistChkB->setChecked( false );
            GenArtistChkB->setDisabled( true );
        }
        if ( formatstr.find( "<album>" ) != -1 ) {
            GenAlbumChkB->setChecked( false );
            GenAlbumChkB->setDisabled( true );
        }
        if ( formatstr.find( "<title>" ) != -1 ) {
            GenTitleChkB->setChecked( false );
            GenTitleChkB->setDisabled( true );
        }
        if ( formatstr.find( "<year>" ) != -1 ) {
            GenYearChkB->setChecked( false );
            GenYearChkB->setDisabled( true );
        }
        if ( formatstr.find( "<comment>" ) != -1 ) {
            GenCommentChkB->setChecked( false );
            GenCommentChkB->setDisabled( true );
        }
        if ( formatstr.find( "<track>" ) != -1 ) {
            GenTrackChkB->setChecked( false );
            GenTrackChkB->setDisabled( true );
        }
    }
    else {
            if ( !GenArtistChkB->isEnabled() ) {
                GenArtistChkB->setDisabled( false );
                GenArtistChkB->setChecked( true );
            }
            if ( !GenAlbumChkB->isEnabled() ) {
                GenAlbumChkB->setDisabled( false );
                GenAlbumChkB->setChecked( true );
            }
            if ( !GenTitleChkB->isEnabled() ) {
                GenTitleChkB->setDisabled( false );
                GenTitleChkB->setChecked( true );
            }
            if ( !GenYearChkB->isEnabled() ) {
                GenYearChkB->setDisabled( false );
                GenYearChkB->setChecked( true );
            }
            if ( !GenCommentChkB->isEnabled() ) {
                GenCommentChkB->setDisabled( false );
                GenCommentChkB->setChecked( true );
            }
            if ( !GenTrackChkB->isEnabled() ) {
                GenTrackChkB->setDisabled( false );
                GenTrackChkB->setChecked( true );
            }
    }
}

void mttMainWin::slotRenameFiles()
{
    int count, current;
    int noall = false;

    QApplication::setOverrideCursor( QCursor( Qt::busyCursor ) );
    statusBar()->message( tr( QString( "Renaming Files..." ) ) );
    count = GenListView->childCount();
    current = 1;
    progress.show();
    progress.setProgress( 0, count );

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        QString newfname, path, ext, cformat;
        TagLib::Tag *t;

        t = ( (AListViewItem *) it.current() )->getTag();
        path = ( (AListViewItem *) it.current() )->getFName();
        ext = path;
        qDebug( "filename:" + path );
        path.truncate( path.findRev( "/" ) );
        ext = ext.right( ext.length() - ext.findRev( "." ) );
        cformat = MCFormatLE->text();

        if ( cformat != "" ) {
            newfname = "";
            bool done = false;
            while ( !done ) {
                if ( cformat.startsWith( "<artist>" ) )
                    newfname += TStringToQString( t->artist() );
                if ( cformat.startsWith( "<album>" ) )
                    newfname += TStringToQString( t->album() );
                if ( cformat.startsWith( "<title>" ) )
                    newfname += TStringToQString( t->title() );
                if ( cformat.startsWith( "<track>" ) ) {
                    if ( LZ )
                        if ( t->track() < 10 )
                            newfname += LZ1;
                        else
                            newfname += LZ2;
                    newfname += QString::number( t->track() );
                }
                if ( cformat.startsWith( "<year>" ) )
                    newfname += QString::number( t->year() );
                if ( cformat.startsWith( "<comment>" ) )
                    newfname += TStringToQString( t->comment() );
                cformat.remove( 0, cformat.find( ">" ) + 1 );

                if ( cformat == "" )
                    done = true;
                else {
                    newfname += cformat.left( cformat.find( "<" ) );
                    cformat.remove( 0, cformat.find( "<" ) );
                    if ( cformat == "" )
                        done = true;
                }
            }
            qDebug( "path:" + path );
            qDebug( "ext:" + ext );
            qDebug( "new filename:" + path + "/" + newfname + ext );
            QDir dir;
            QFile f( path + "/" + newfname + ext );
            if ( !noall ) {
                if ( f.exists() ) { // File with that name already exists
                    switch( QMessageBox::question(
                                this,
                                tr("Overwrite File? -- Media Tag Tools"),
                                tr("A file called %1 already exists. "
                                "Do you want to overwrite it?")
                                .arg( f.name() ),
                                tr("&Yes"), tr("&No"),
                                tr("No to &All"), 0, 1 ) ) {
                        case 0:
                            dir.rename( ( (AListViewItem *) it.current() )->getFName() , path + "/" + newfname + ext );
                        case 1:
                            break;
                        case 2:
                            noall = true;
                            break;
                    }
                }
                else
                    dir.rename( ( (AListViewItem *) it.current() )->getFName() , path + "/" + newfname + ext );
            }
        }
        ++it;
        progress.setProgress( current++, count );
    }

    GenListView->clear();
    d.refresh();
    populateList();

    progress.hide();
    statusBar()->message( tr( QString( "Done" ) ) );
    QApplication::restoreOverrideCursor();
}

void mttMainWin::slotAbout()
{
    mttAboutDialog about;
    about.exec();
}

void mttMainWin::slotCorrectCase()
{
    QPopupMenu menu;
    menu.insertItem( tr( "First letter up" ), this, SLOT(slotFirstUp()) );
    menu.insertItem( tr( "All uppercase" ), this, SLOT(slotAllUpper()) );
    menu.insertItem( tr( "All lowercase" ), this, SLOT(slotAllLower()) );
    menu.exec( QCursor::pos() );
}

void mttMainWin::slotAllUpper()
{
    GenTitleCLE->setText( GenTitleCLE->text().upper() );
    GenArtistCLE->setText( GenArtistCLE->text().upper() );
    GenAlbumCLE->setText( GenAlbumCLE->text().upper() );
    GenGenreCB->setCurrentText( GenGenreCB->currentText().upper() );
    GenCommentCLE->setText( GenCommentCLE->text().upper() );
}

void mttMainWin::slotAllLower()
{
    GenTitleCLE->setText( GenTitleCLE->text().lower() );
    GenArtistCLE->setText( GenArtistCLE->text().lower() );
    GenAlbumCLE->setText( GenAlbumCLE->text().lower() );
    GenGenreCB->setCurrentText( GenGenreCB->currentText().lower() );
    GenCommentCLE->setText( GenCommentCLE->text().lower() );
}

void mttMainWin::slotFirstUp()
{
    GenTitleCLE->setText( firstUp( GenTitleCLE->text().lower() ) );
    GenArtistCLE->setText( firstUp( GenArtistCLE->text().lower() ) );
    GenAlbumCLE->setText( firstUp( GenAlbumCLE->text().lower() ) );
    GenGenreCB->setCurrentText( firstUp( GenGenreCB->currentText().lower() ) );
    GenCommentCLE->setText( firstUp( GenCommentCLE->text().lower() ) );
}

void mttMainWin::slotEmptyFields()
{
    GenTitleCLE->clear();
    GenArtistCLE->clear();
    GenAlbumCLE->clear();
    GenYearCLE->clear();
    GenGenreCB->clearEdit();
    GenCommentCLE->clear();
    GenTrackCLE->clear();
}

QString mttMainWin::firstUp( QString str )
{
    unsigned int i;

    i = 0;
    while ( i < str.length() ) {
        if ( i == 0 && str[i].isLetter() ) {
            str[i] = str[i].upper();
        }
        if ( str[i].isSpace() && ( ( i + 1 ) < str.length() ) && str[i+1].isLetter() ) {
            str[i+1] = str[i+1].upper();
        }
        i++;
    }

    return str;
}

void mttMainWin::slotLVRightMenu()
{
    QPopupMenu menu;
    menu.insertItem( tr( "Open folder" ), this, SLOT(slotOpen()) );
    menu.insertItem( tr( "Add file(s)" ), this, SLOT(slotOpenFiles()) );
    menu.insertSeparator();
    menu.insertItem( tr( "Write tags" ), this, SLOT(slotSaveTags()) );
    menu.insertItem( tr( "Write selected tags only" ), this, SLOT(slotSaveSelectedTags()) );
    menu.insertItem( tr( "Remove tag" ), this, SLOT(slotRemoveTags()) );
    menu.insertItem( tr( "Fix tag (iso->utf8)" ), this, SLOT(slotFixTags()) );
    menu.exec( QCursor::pos() );
}

void mttMainWin::slotFixTags()
{
    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        if ( ( (AListViewItem *) it.current() )->isMpeg() ) {
            TagLib::ID3v2::Tag *tag = ( (AListViewItem *) it.current() )->getID3Tag();
            TagLib::ID3v2::FrameList l = tag->frameList();
            TagLib::List<TagLib::ID3v2::Frame *>::Iterator fit;

            for ( fit=l.begin(); fit != l.end(); fit++ ) {
                if ( *( (*fit)->frameID().data() ) == 'T' ) {
                    TagLib::ID3v2::TextIdentificationFrame *f = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *> (*fit);

                    if ( f ) {
                        qDebug( QString::number( f->textEncoding() ) );
                        qDebug( f->toString().toCString( false ) );
                        TagLib::String fixed( f->toString().toCString(), TagLib::String::UTF8 );
                        if ( f->textEncoding() == TagLib::String::Latin1 ) { // latin1
                            f->setTextEncoding( TagLib::String::UTF8 );
                            f->setText( fixed );
                        }
                    }
                    else
                       qDebug( "fix = NULL" );
                } // end if frameID starts with T
            } // for each text frame
            ( (AListViewItem *) it.current() )->saveTag();
        } // end if is Mpeg file
        ++it;
    } // for each selected item
    GenListView->clear();
    populateList();
}

void mttMainWin::slotTitleEnter()
{
    if ( GenArtistCLE->isEnabled() )
        GenArtistCLE->setFocus();
    else if ( GenAlbumCLE->isEnabled() )
        GenAlbumCLE->setFocus();
    else if ( GenYearCLE->isEnabled() )
        GenYearCLE->setFocus();
//    else if ( GenGenreCB->isEnabled() )
//        GenGenreCB->setFocus();
    else if ( GenCommentCLE->isEnabled() )
        GenCommentCLE->setFocus();
    else if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotArtistEnter()
{
    if ( GenAlbumCLE->isEnabled() )
        GenAlbumCLE->setFocus();
    else if ( GenYearCLE->isEnabled() )
        GenYearCLE->setFocus();
/*    else if ( GenGenreCB->isEnabled() )
        GenGenreCB->setFocus();*/
    else if ( GenCommentCLE->isEnabled() )
        GenCommentCLE->setFocus();
    else if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotAlbumEnter()
{
    if ( GenYearCLE->isEnabled() )
        GenYearCLE->setFocus();
/*    else if ( GenGenreCB->isEnabled() )
        GenGenreCB->setFocus();*/
    else if ( GenCommentCLE->isEnabled() )
        GenCommentCLE->setFocus();
    else if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotYearEnter()
{
/*    if ( GenGenreCB->isEnabled() )
        GenGenreCB->setFocus();*/
    if ( GenCommentCLE->isEnabled() )
        GenCommentCLE->setFocus();
    else if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotGenreEnter()
{
    if ( GenCommentCLE->isEnabled() )
        GenCommentCLE->setFocus();
    else if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotCommentEnter()
{
    if ( GenTrackCLE->isEnabled() )
        GenTrackCLE->setFocus();
    else
        saveTags( true );
}

void mttMainWin::slotNextEntry()
{
    QListViewItem *c;
    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );

    c = it.current();
    if ( c ) {
        if ( c->itemBelow() ) {
            GenListView->clearSelection();
            GenListView->setSelected( c->itemBelow(), TRUE );
            GenListView->ensureItemVisible( c->itemBelow() );
        }
    }
    else
        GenListView->setSelected( GenListView->firstChild(), TRUE );
}

void mttMainWin::slotPreviousEntry()
{
    QListViewItem *c;
    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );

    c = it.current();
    if ( c ) {
        if ( c->itemAbove() ) {
            GenListView->clearSelection();
            GenListView->setSelected( c->itemAbove(), TRUE );
            GenListView->ensureItemVisible( c->itemAbove() );
        }
    }
    else
        GenListView->setSelected( GenListView->firstChild(), TRUE );
}

void mttMainWin::slotPreviousPage()
{
}

void mttMainWin::slotNextPage()
{
}

void mttMainWin::slotTitleChanged( QString title )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( TStringToQString( t->title() ) != title ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setTitle( QStringToTString( title ) );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 1, title );
        }
    it++;
    }
}

void mttMainWin::slotSelectionChange()
{
    bool updateSelectedFname = false;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );

    if ( !selectedFname.isEmpty() ) {
        QListViewItem *lvi = GenListView->findItem( selectedFname, 0 );
        if ( lvi && !lvi->isSelected() )
            updateSelectedFname = true;
    }
    if ( selectedFname.isEmpty() || updateSelectedFname ) {
        // Clear the contents of the widgets
        //slotEmptyFields(); // Probably unecessary

        // Set the contents of the widgets according to the tag
        TagLib::Tag *t;

        if ( it.current() ) {
            // Show tag info
            t = (( AListViewItem *) it.current() )->getTag();
            if ( t ) {
                GenTitleCLE->setText( TStringToQString( t->title() ) );
                GenArtistCLE->setText( TStringToQString( t->artist() ) );
                GenAlbumCLE->setText( TStringToQString( t->album() ) );
                GenYearCLE->setText( QString::number( t->year() ) );
                GenGenreCB->setCurrentText( TStringToQString( t->genre() ) );
                GenCommentCLE->setText( TStringToQString( t->comment() ) );
                GenTrackCLE->setText( QString::number( t->track() ) );

                selectedFname = (( AListViewItem *) it.current() )->getFName();
                selectedFname = selectedFname.mid( selectedFname.findRev( "/" ) + 1 );
            }
            // Show media info
        }
    }
}

void mttMainWin::slotTrackChanged( QString track )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if (  t->track() != track.toUInt() ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setTrack( track.toUInt() );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 7, track );
        }
    it++;
    }
}


void mttMainWin::slotCommentChanged( QString comment )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( TStringToQString( t->comment() ) != comment ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setComment( QStringToTString( comment ) );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 6, comment );
        }
    it++;
    }
}


void mttMainWin::slotYearChanged( QString year )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( t->year() != year.toUInt() ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setYear( year.toUInt() );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 4, year );
        }
    it++;
    }
}


void mttMainWin::slotAlbumChanged( QString album )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( TStringToQString( t->album() ) != album ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setAlbum( QStringToTString( album ) );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 3, album );
        }
    it++;
    }
}


void mttMainWin::slotArtistChanged( QString artist )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( TStringToQString( t->artist() ) != artist ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setArtist( QStringToTString( artist ) );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 2, artist );
        }
    it++;
    }
}

void mttMainWin::slotGenreChanged( const QString &genre )
{
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        if ( TStringToQString( t->genre() ) != genre ) { // A bit of a double check but just to be sure...
            ( (AListViewItem *) it.current() )->checkEncodings();
            // Save info from the various text fields
            t->setGenre( QStringToTString( genre ) );
            ( (AListViewItem *) it.current() )->setTagChanged( true );
            it.current()->setText( 5, genre );
        }
    it++;
    }
}

void mttMainWin::saveTags( bool selectedOnly )
{
    TagLib::Tag *t;
    int current, count;

    QApplication::setOverrideCursor( QCursor( Qt::busyCursor ) );
    statusBar()->message( tr( QString( "Writing tags..." ) ) );
    count = GenListView->childCount();
    current = 1;
    progress.show();
    progress.setProgress( 0, count );
    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);
    QListViewItemIterator it;
    if ( selectedOnly ) {
        QListViewItemIterator tmp( GenListView, QListViewItemIterator::Selected );
        it = tmp;
        }
    else {
        QListViewItemIterator tmp( GenListView );
        it = tmp;
        }
    while ( it.current() ) {
        if ( selectedOnly || ( (AListViewItem *) it.current() )->tagChanged() ) {
            t = ( (AListViewItem *) it.current() )->getTag();
            ( (AListViewItem *) it.current() )->checkEncodings();

            ( (AListViewItem *) it.current() )->saveTag();
            ( (AListViewItem *) it.current() )->setTagChanged( false );

            // Update the ListView too
            ( (AListViewItem *) it.current() )->repaint();
        }
        ++it;

        progress.setProgress( current++, count );
    }

    QApplication::restoreOverrideCursor();
    progress.hide();
    statusBar()->message( tr( QString( "Done" ) ) );
}

void mttMainWin::slotCreateTags()
{
    TagLib::Tag *t;
    int current, count;

    QApplication::setOverrideCursor( QCursor( Qt::busyCursor ) );
    statusBar()->message( tr( QString( "Writing tags..." ) ) );
    count = GenListView->childCount();
    current = 1;
    progress.show();
    progress.setProgress( 0, count );
    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);
    QListViewItemIterator it;
    QListViewItemIterator tmp( GenListView, QListViewItemIterator::Selected );
    it = tmp;
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();
        ( (AListViewItem *) it.current() )->checkEncodings();

        // Save info using the filename and the custom format string
        if ( UseCFChkB->isChecked() && ( MCFormatLE->text() != "" ) ) { // Custom format is enabled
            QString formatstr = MCFormatLE->text();
            QStringList::Iterator slit = separators.begin();
            QString filename = ( (AListViewItem *) it.current() )->getFName();
            int curpos, pos1, pos2;
            bool done;
    
            // Remove path from filename
            curpos = filename.findRev( "/" );
            if ( curpos != -1 )
                filename.remove( 0, curpos + 1 );
            // Remove extension too
            curpos = filename.findRev( "." );
            if ( curpos != -1 )
                filename.truncate( curpos );
    
            done = false;
            while ( !done ) {
                pos1 = formatstr.find( ">" );
                pos2 = formatstr.find( "<", 1 );
                if ( pos1 == -1 ) // No more <> tags
                    done = true;
                else if ( pos2 == -1 ) { // Last <> tag
                    if ( formatstr.startsWith( QString( "<artist>" ) ) )
                        t->setArtist( QStringToTString( filename ) );
                    if ( formatstr.startsWith( QString( "<album>" ) ) )
                        t->setAlbum( QStringToTString( filename ) );
                    if ( formatstr.startsWith( QString( "<year>" ) ) )
                        t->setYear( filename.toInt() );
                    if ( formatstr.startsWith( QString( "<title>" ) ) )
                        t->setTitle( QStringToTString( filename ) );
                    if ( formatstr.startsWith( QString( "<comment>" ) ) )
                        t->setComment( QStringToTString( filename ) );
                    if ( formatstr.startsWith( QString( "<genre>" ) ) )
                        t->setGenre( QStringToTString( filename ) );
                    if ( formatstr.startsWith( QString( "<track>" ) ) )
                        t->setTrack( filename.toInt() );
                    done = true;
                }
                else { // <> tag & separator exist
                    int pos = filename.find( formatstr.mid( pos1 + 1, pos2 - pos1 -1 ) );
                    if ( pos != -1 ) {
                        QString data = filename.left( pos );

                        if ( formatstr.startsWith( QString( "<artist>" ) ) )
                            t->setArtist( QStringToTString( data ) );
                        if ( formatstr.startsWith( QString( "<album>" ) ) )
                            t->setAlbum( QStringToTString( data ) );
                        if ( formatstr.startsWith( QString( "<year>" ) ) )
                            t->setYear( data.toInt() );
                        if ( formatstr.startsWith( QString( "<title>" ) ) )
                            t->setTitle( QStringToTString( data ) );
                        if ( formatstr.startsWith( QString( "<comment>" ) ) )
                            t->setComment( QStringToTString( data ) );
                        if ( formatstr.startsWith( QString( "<genre>" ) ) )
                            t->setGenre( QStringToTString( data ) );
                        if ( formatstr.startsWith( QString( "<track>" ) ) )
                            t->setTrack( data.toInt() );

                        //qDebug( "filename:" + filename );
                        //qDebug( "formatstr:" + formatstr );
                        //qDebug( "data:" + data );
                        //qDebug( "mid:" + formatstr.mid( pos1 + 1, pos2 - pos1 - 1 ) );
                        filename.remove( 0, pos + pos2 - pos1 - 1 );
                        formatstr.remove( 0, pos2 );
                        //qDebug( "filename:" + filename );
                        //qDebug( "formatstr:" + formatstr );
                    }
                    else
                         done = true;
                }
            }
 
            ( (AListViewItem *) it.current() )->setTagChanged( true );

            // Update the ListView too
            ( (AListViewItem *) it.current() )->repaint();
            ( (AListViewItem *) it.current() )->setText( 1, TStringToQString( t->title() ) );
            ( (AListViewItem *) it.current() )->setText( 2, TStringToQString( t->artist() ) );
            ( (AListViewItem *) it.current() )->setText( 3, TStringToQString( t->album() ) );
            ( (AListViewItem *) it.current() )->setText( 4, QString::number( t->year() ) );
            ( (AListViewItem *) it.current() )->setText( 5, TStringToQString( t->genre() ) );
            ( (AListViewItem *) it.current() )->setText( 6, TStringToQString( t->comment() ) );
            ( (AListViewItem *) it.current() )->setText( 7, QString::number( t->track() ) );
        }
        ++it;

        progress.setProgress( current++, count );
    }

    QApplication::restoreOverrideCursor();
    progress.hide();
    statusBar()->message( tr( QString( "Done" ) ) );
}



