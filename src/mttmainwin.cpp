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

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include <tag.h>
#include <tstring.h>
#include <id3v2framefactory.h>

#include "mttmainwin.h"
#include "alistviewitem.h"
#include "mttcfdialog.h"
#include "revision.h"

mttMainWin::mttMainWin(QWidget* parent, const char* name, WFlags fl)
: MainForm(parent,name,fl)
{
    int i;

    d.setFilter( QDir::Files | QDir::Readable );
    d.setNameFilter( QString( "*.mp3;*.ogg;*.flac" ) );

    for ( i=0; i<5; i++ ) {
        separators << " - ";
    }
    setCaption( caption() + " - " + RV_SNAPSHOT_VERSION );
    /*
    QPopupMenu *fileMenu = new QPopupMenu( this );
    fileMenu->insertItem( "Open", this, SLOT( slotOpen() ),    CTRL+Key_O );
    fileMenu->insertSeparator();
    fileMenu->insertItem( "Exit", this, SLOT( close() ),    CTRL+Key_X );

    QPopupMenu *editMenu = new QPopupMenu( this );
    editMenu->insertItem( "Preferences", this, SLOT( preferences() ), CTRL+Key_P );

    QPopupMenu *helpMenu = new QPopupMenu( this );
    helpMenu->insertItem( "Help", this, SLOT( help() ) );
    helpMenu->insertSeparator();
    helpMenu->insertItem( "About", this, SLOT( about() ) );

    menu = new QMenuBar( this );
    menu->insertItem( "&File", fileMenu );
    menu->insertItem( "&Edit", editMenu );
    menu->insertItem( "&Help", helpMenu );
    MainFormLayout->addWidget( menu );*/
}

mttMainWin::~mttMainWin()
{
}

void mttMainWin::slotOpen()
{
    QFileDialog fd;
    bool done = false;

    fd.setMode( QFileDialog::Directory );
    fd.addFilter( "Audio Files (*.mp3 *.ogg *.flac)" );
    fd.setDir( d.path() );
    while (!done) {
        if ( fd.exec() == QDialog::Accepted ) {
            fprintf(stderr, "%s\n", fd.selectedFile().ascii() );
            d.setPath( QString( fd.selectedFile() ) );
            if ( d.exists() )
                done = true;
            else
                QMessageBox::critical( this, "Error", "Dir "+QString( fd.selectedFile() )+" not found!", QMessageBox::Ok, QMessageBox::NoButton );
        }
        else
            return;
    }

    GenListView->clear();
    populateList();
}

void mttMainWin::populateList( void )
{
    QStringList fnames;
    AListViewItem *li;
    TagLib::Tag *t;

    fnames = d.entryList();

    for ( QStringList::Iterator it = fnames.begin(); it != fnames.end(); ++it ) {
        li = new AListViewItem( GenListView );
        li->setText( 0, *it );
        qDebug( d.path() + "/" + *it );
        if ( QFile::exists( d.path() + "/" + *it ) )
            qDebug( "exists" );
        else
            qDebug( "doesn't exist" );
        li->FileRef( QStringToTString( QString( d.path() + "/" + *it ) ).to8Bit( true ) );
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
    }
}

void mttMainWin::slotClickOnItem( QListViewItem* item )
{
    // Clear the contents of the widgets
    GenTitleLE->clear();
    GenArtistLE->clear();
    GenAlbumLE->clear();
    GenYearLE->clear();
    GenGenreCB->clearEdit();
    GenCommentLE->clear();
    GenTrackLE->clear();

    // Set the contents of the widgets according to the tag
    TagLib::Tag *t;

    if ( item ) {
        t = (( AListViewItem *) item )->getTag();
        if ( t ) {
            GenTitleLE->setText( TStringToQString( t->title() ) );
            GenArtistLE->setText( TStringToQString( t->artist() ) );
            GenAlbumLE->setText( TStringToQString( t->album() ) );
            GenYearLE->setText( QString::number( t->year() ) );
            GenGenreCB->setCurrentText( TStringToQString( t->genre() ) );
            GenCommentLE->setText( TStringToQString( t->comment() ) );
            GenTrackLE->setText( QString::number( t->track() ) );
        }
    }
}

void mttMainWin::slotSaveTags()
{
    TagLib::Tag *t;

    TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);
    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        t = ( (AListViewItem *) it.current() )->getTag();

        // Save info from the various text fields
        if ( GenTitleChkB->isEnabled() && GenTitleChkB->isChecked() )
            t->setTitle( QStringToTString( GenTitleLE->text() ) );
        if ( GenArtistChkB->isEnabled() && GenArtistChkB->isChecked() )
            t->setArtist( QStringToTString( GenArtistLE->text() ) );
        if ( GenAlbumChkB->isEnabled() && GenAlbumChkB->isChecked() )
            t->setAlbum( QStringToTString( GenAlbumLE->text() ) );
        if ( GenYearChkB->isEnabled() && GenYearChkB->isChecked() )
            t->setYear( GenYearLE->text().toInt() );
        if ( GenGenreChkB->isEnabled() && GenGenreChkB->isChecked() )
            t->setGenre( QStringToTString( GenGenreCB->currentText() ) );
        if ( GenCommentChkB->isEnabled() && GenCommentChkB->isChecked() )
            t->setComment( QStringToTString( GenCommentLE->text() ) );
        if ( GenTrackChkB->isEnabled() && GenTrackChkB->isChecked() )
            t->setTrack( GenTrackLE->text().toInt() );

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

                        qDebug( "filename:" + filename );
                        qDebug( "formatstr:" + formatstr );
                        qDebug( "data:" + data );
                        qDebug( "mid:" + formatstr.mid( pos1 + 1, pos2 - pos1 - 1 ) );
                        filename.remove( 0, pos + pos2 - pos1 - 1 );
                        formatstr.remove( 0, pos2 );
                        qDebug( "filename:" + filename );
                        qDebug( "formatstr:" + formatstr );
                    }
                    else
                        done = true;
                }
            }
        }

        ( (AListViewItem *) it.current() )->saveTag();
        ++it;
    }

    GenListView->clear();
    populateList();
}

void mttMainWin::slotRemoveTags()
{
    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        ( (AListViewItem *) it.current() )->removeTag();
        ++it;
    }

    GenListView->clear();
    populateList();
}

void mttMainWin::slotCFormat()
{
    mttCFDialog cfdialog;

    cfdialog.setFormat( MCFormatLE->text() );
    cfdialog.setSeparators( separators );
    if ( cfdialog.exec() == QDialog::Accepted ) {
        MCFormatLE->setText( cfdialog.getFormat() );
        separators = cfdialog.getSeparators();
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
    TagLib::Tag *t;

    QListViewItemIterator it( GenListView, QListViewItemIterator::Selected );
    while ( it.current() ) {
        QString newfname, path, ext, cformat;

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
                if ( cformat.startsWith( "<track>" ) )
                    newfname += QString::number( t->track() );
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
            dir.rename( ( (AListViewItem *) it.current() )->getFName(), path + "/" + newfname + ext );
        }
        ++it;
    }

    GenListView->clear();
    d.refresh();
    populateList();
}








/*$SPECIALIZATION$*/


