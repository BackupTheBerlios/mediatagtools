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

#include <QtGui/QFileDialog>
#include <QtGui/QPixmap>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QMessageBox>
#include <QtGui/QCursor>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtCore/QList>
#include <QtGui/QFrame>
#include <QtGui/QFormLayout>

#include <fileref.h>
#include <tag.h>
#include <tlist.h>
#include <tstring.h>
#include <id3v2framefactory.h>
#include <id3v2tag.h>
#include <id3v1genres.h>
#include <textidentificationframe.h>
#include <tstringlist.h>

//#include "qclineedit.h"
//#include "qdndlistview.h"
#include "mttmainwin.h"
#include "mttitemdelegate.h"
#include "mttfile.h"
//#include "mttcfdialog.h"
//#include "mttaboutdialog.h"
#include "config.h"
//#include "x.xpm"
#include "tools.h"

#ifndef RELEASE
#include "revision.h"
#endif

//#include "genres.h"
//#include "mp3extraframes.h"

#include "modeltest.h"

mttMainWin::mttMainWin(QWidget* parent) : QMainWindow( parent )
{
    setupUi( this );
    int i;

    /*QStringList strlst( "<Empty>" );*/

    ignoreChange = false;

    /*GenGenreCB->insertStrList( genres );
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

    // Initialization of TreeView
    treeView = new QTreeView( this );
    setCentralWidget( treeView );
    treeView->setModel( &treeModel );
    treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    treeView->setSelectionBehavior( QAbstractItemView::SelectRows );
    //treeView->setDragEnabled( true );
    //treeView->setTabKeyNavigation( true );
    //treeView->setAllColumnsShowFocus( true );
    treeView->setRootIsDecorated( true );
    //treeView->setItemDelegate( (QAbstractItemDelegate*) new mttItemDelegate() );
    treeView->show();

    QStringList header;
    header << tr( "Filename" ) << tr( "Title" ) << tr( "Artist" ) << tr( "Album" )
           << tr( "Year" ) << tr( "Genre" ) << tr( "Comment" ) << tr( "Track" );
    treeModel.setHorizontalHeaderLabels( header );

    // Signal & Slot connections for TreeView
	connect( treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotSelectionChange(const QModelIndex&, const QModelIndex&)) );
	connect( treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(slotSelectionChange(const QItemSelection&, const QItemSelection&)) );

    // Progress bar & Status Bar
    progress.setRange( 0, 100 );
    progress.setValue( 100 );
    statusBar()->showMessage( tr( "Ready" ) );

	// Add Dock Widgets
	// ----------------
	// Detail Dock
	QDockWidget *dockDetails;
	QFrame *detailFrame;
	QFormLayout *detailLayout;

	dockDetails = new QDockWidget( tr("Details"), this );
	detailFrame = new QFrame( dockDetails );
	detailLayout = new QFormLayout( detailFrame );

	lengthLabel = new QLabel();
	detailLayout->addRow( tr("Length:"), lengthLabel );
	bitrateLabel = new QLabel();
	detailLayout->addRow( tr("Bitrate:"), bitrateLabel );
	sampleRateLabel = new QLabel();
	detailLayout->addRow( tr("Sample Rate:"), sampleRateLabel );
	channelLabel = new QLabel();
	detailLayout->addRow( tr("Channels:"), channelLabel );
	detailFrame->setLayout( detailLayout );
	dockDetails->setWidget( detailFrame );
	this->addDockWidget( Qt::RightDockWidgetArea, dockDetails );

	// Edit Dock
	QDockWidget *dockEdit;
	QFrame *editFrame;
	QFormLayout *formLayout;
	dockEdit = new QDockWidget( tr("Edit"), this );
	editFrame = new QFrame(dockEdit);
	formLayout = new QFormLayout( editFrame );

	titleEdit = new QLineEdit( editFrame );
	formLayout->addRow( tr("Title"), titleEdit );
	artistEdit = new QLineEdit( editFrame );
	formLayout->addRow( tr("Artist"), artistEdit );
	albumEdit = new QLineEdit( editFrame );
	formLayout->addRow( tr("Album"), albumEdit );
	yearEdit = new QLineEdit( editFrame );
	yearEdit->setInputMask( "0000" );
	formLayout->addRow( tr("Year"), yearEdit );
	genreEdit = new QComboBox( editFrame );
	genreEdit->setDuplicatesEnabled( false );
	genreEdit->setEditable( true );
	TagLib::StringList sl;
	QStringList qsl;
	sl = TagLib::ID3v1::genreList();
	for (unsigned int i=0;i<sl.size();i++)
		qsl << TStringToQString( sl[i] );
	genreEdit->insertItems( 0, qsl );
	formLayout->addRow( tr("Genre"), genreEdit );
	commentEdit = new QLineEdit( editFrame );
	formLayout->addRow( tr("Comment"), commentEdit );
	trackEdit = new QLineEdit( editFrame );
	trackEdit->setInputMask( "0000" );
	formLayout->addRow( tr("Track"), trackEdit );
	
	editFrame->setLayout( formLayout );
	dockEdit->setWidget( editFrame );
	this->addDockWidget( Qt::RightDockWidgetArea, dockEdit );
	tabifyDockWidget( dockDetails, dockEdit );

	// Renumber dock
	QDockWidget *dockRenum;
	dockRenum = new QDockWidget( tr("Renumber"), this );
	renumModel.setSupportedDragActions( Qt::CopyAction | Qt::MoveAction );
	tableView = new QTableView( this );
	tableView->setModel( &renumModel );
	tableView->setAlternatingRowColors( true );
	tableView->setSelectionMode( QAbstractItemView::SingleSelection );
	tableView->setDragEnabled( true );
	tableView->setAcceptDrops( true );
	tableView->setDropIndicatorShown( true );
	tableView->setDragDropMode( QAbstractItemView::InternalMove );
	tableView->setSelectionBehavior( QAbstractItemView::SelectRows );
	tableView->setSortingEnabled( false );
	dockRenum->setWidget( tableView );
	this->addDockWidget( Qt::RightDockWidgetArea, dockRenum );

	// Signal & Slot connections for dock widgets
	connect( titleEdit, SIGNAL( textEdited(const QString&) ), this, SLOT( slotTitleChanged(const QString&) ) );
	connect( artistEdit, SIGNAL( textEdited(const QString&) ), this, SLOT( slotArtistChanged(const QString&) ) );
	connect( albumEdit, SIGNAL( textEdited(const QString&) ), this, SLOT( slotAlbumChanged(const QString&) ) );
	connect( yearEdit, SIGNAL( textEdited(const QString&) ), this, SLOT( slotYearChanged(const QString&) ) );
	connect( trackEdit, SIGNAL( textEdited(const QString&) ), this, SLOT( slotTrackChanged(const QString&) ) );
	connect( genreEdit, SIGNAL( editTextChanged(const QString&) ), this, SLOT( slotGenreChanged(const QString&) ) );

	connect( titleEdit, SIGNAL( returnPressed() ), this, SLOT( slotTitleEnter() ) );
	connect( artistEdit, SIGNAL( returnPressed() ), this, SLOT( slotArtistEnter() ) );
	connect( albumEdit, SIGNAL( returnPressed() ), this, SLOT( slotAlbumEnter() ) );
	connect( yearEdit, SIGNAL( returnPressed() ), this, SLOT( slotYearEnter() ) );
	connect( commentEdit, SIGNAL( returnPressed() ), this, SLOT( slotCommentEnter() ) );
	//connect( trackEdit, SIGNAL( returnPressed() ), this, SLOT( slotSaveTags() ) );

    // Signal & Slot connections for menubar
    actionOpen_folder->setShortcut( tr( "Ctrl+O" ) );
    actionExit->setShortcut( tr( "Alt+X" ) );
    connect(actionOpen_folder, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(actionOpen_files, SIGNAL(triggered()), this, SLOT(slotOpenFiles()));

    // Create list for taglib known filetypes
    TagLib::StringList tl = TagLib::FileRef::defaultFileExtensions();
    for ( unsigned int i=0; i<tl.size(); i++ ) {
        knownFiletypes << QString( "*." ) + TStringToQString( tl[i] );
    }

    //new ModelTest(&treeModel, this);
}

mttMainWin::~mttMainWin()
{
}

void mttMainWin::addDir( QString str )
{
    QDir d;

    d.setPath( str );
    d.setFilter( QDir::Files | QDir::Readable );
    d.setNameFilters( knownFiletypes );

    if ( d.exists() && d.isReadable() ) {
        curPath = d.path();
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        populateList( d );
        QApplication::restoreOverrideCursor();
    }
    selectedFname = "";
}

void mttMainWin::addFile( QString &fname )
{
    mttFile *li;
    TagLib::Tag *t;
    QList<QStandardItem *> fatherlist, newrow;
	QStandardItem *father;
    QList<QVariant> list;
    bool itemChanged;

    li = new mttFile();
    itemChanged = li->Open( fname );

	qDebug( fname.toUtf8().constData() );
	// Search if another file was opened from the same path
	fatherlist = treeModel.findItems( curPath );
	// If not then create a new branch
    if ( fatherlist.empty() ) {
		father = treeModel.invisibleRootItem();
		QStandardItem *item = new QStandardItem( curPath );
		father->appendRow( item );
		father = item;
    }
	else
		father = fatherlist[0];

	newrow << new QStandardItem( fname.right( fname.size() - curPath.size() -1 ) );
    t = li->getTag();
    if ( t ) {
       newrow << new QStandardItem( TStringToQString( t->title() ) )
            << new QStandardItem( TStringToQString( t->artist() ) )
            << new QStandardItem( TStringToQString( t->album() ) )
            << new QStandardItem( QString::number( t->year() ) )
            << new QStandardItem( TStringToQString( t->genre() ) )
            << new QStandardItem( TStringToQString( t->comment() ) )
            << new QStandardItem( QString::number( t->track() ) );
    }

	father->appendRow( newrow );

	// If the file had id3v1 tag that was converted to id3v2 make it different from the others
    if ( itemChanged )
		newrow[0]->setForeground( QBrush( QColor( Qt::red ) ) );
}

void mttMainWin::slotOpen()
{
    QFileDialog fd;
    QDir d;
    bool done = false;

    d.setFilter( QDir::Files | QDir::Readable );
    d.setNameFilters( knownFiletypes );

    fd.setFileMode( QFileDialog::Directory );
    QStringList filters = fd.filters();
    fd.setDirectory( curPath );
    while (!done) {
        if ( fd.exec() == QDialog::Accepted ) {
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
    populateList( d );
    QApplication::restoreOverrideCursor();
    selectedFname = "";
}

void mttMainWin::slotOpenFiles()
{
    QFileDialog fd;
    QStringList files;

    int count = 0, current = 1;

    fd.setFileMode( QFileDialog::ExistingFiles );
    QStringList filters = fd.filters();
    filters = QStringList() << QString( tr( "Audio Files (" ) ) + knownFiletypes.join( " " ) + ")" << filters;
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
		//qDebug( QString::number( current ).toUtf8().constData() );
		//qDebug( QString::number( treeModel.rowCount() ).toUtf8().constData() 
		QString fn;
		fn = curPath + "/" + *it;
        addFile( fn );
        progress.setValue( current++ );
    }

	//qDebug( QString::number( treeModel.rowCount() ).toUtf8().constData() );
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

void mttMainWin::slotAllUpper()
{
    QList<QModelIndex> list;
//     TreeItem *ti;
    int i;

    list = treeView->selectionModel()->selectedIndexes();

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

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

    for ( i = 0; i < list.size(); i+=8 ) { // 8 is the number of columns in TreeView
        QList<QVariant> l;

//         ti = (TreeItem *) list.at(i).internalPointer();
// 		ti->setItemChanged( true );

        l << treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole )
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toUpper()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toUpper()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toUpper()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toUpper()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toUpper()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString();
        treeModel.setData( list.at(i), l );
    }

    QApplication::restoreOverrideCursor();
}

void mttMainWin::slotAllLower()
{
    QList<QModelIndex> list;
//     TreeItem *ti;
    int i;

    list = treeView->selectionModel()->selectedIndexes();

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

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

    for ( i = 0; i < list.size(); i+=8 ) { // 8 is the number of columns in TreeView
        QList<QVariant> l;

//         ti = (TreeItem *) list.at(i).internalPointer();
// 		ti->setItemChanged( true );

        l << treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole )
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower()
			<< treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString();
        treeModel.setData( list.at(i), l );
    }

    QApplication::restoreOverrideCursor();
}

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
void mttMainWin::contextMenuEvent( QContextMenuEvent *e )
{
    QMenu menu, corCaseMenu;

    corCaseMenu.setTitle( "Correct Case" );
    corCaseMenu.addAction( tr( "First letter up (first word)" ), this, SLOT(slotFirstUpSentence()) );
    corCaseMenu.addAction( tr( "First letter up (each word)" ), this, SLOT(slotFirstUpWords()) );
    corCaseMenu.addAction( tr( "All uppercase" ), this, SLOT(slotAllUpper()) );
    corCaseMenu.addAction( tr( "All lowercase" ), this, SLOT(slotAllLower()) );

    menu.addAction( tr( "Open folder" ), this, SLOT(slotOpen()) );
    menu.addAction( tr( "Add file(s)" ), this, SLOT(slotOpenFiles()) );
    menu.addAction( tr( "Remove file(s)" ), this, SLOT(slotRemoveFiles()) );
    menu.addSeparator();
    menu.addAction( tr( "Write tags" ), this, SLOT(slotSaveTags()) );
    menu.addAction( tr( "Write selected tags only" ), this, SLOT(slotSaveSelectedTags()) );
    menu.addAction( tr( "Remove tag" ), this, SLOT(slotRemoveTags()) );
    menu.addSeparator();
    menu.addMenu( &corCaseMenu );
    menu.addAction( tr( "Fix tag (iso->utf8)" ), this, SLOT(slotFixTags()) );
    menu.exec( e->globalPos() );
}

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

void mttMainWin::slotTitleEnter()
{
	if ( artistEdit->isEnabled() )
		artistEdit->setFocus();
    else if ( albumEdit->isEnabled() )
        albumEdit->setFocus();
    else if ( yearEdit->isEnabled() )
        yearEdit->setFocus();
    else if ( genreEdit->isEnabled() )
        genreEdit->setFocus();
    else if ( commentEdit->isEnabled() )
        commentEdit->setFocus();
    else if ( trackEdit->isEnabled() )
        trackEdit->setFocus();
//     else
//         saveTags( true );
}

void mttMainWin::slotArtistEnter()
{
    if ( albumEdit->isEnabled() )
        albumEdit->setFocus();
    else if ( yearEdit->isEnabled() )
        yearEdit->setFocus();
    else if ( genreEdit->isEnabled() )
        genreEdit->setFocus();
    else if ( commentEdit->isEnabled() )
        commentEdit->setFocus();
    else if ( trackEdit->isEnabled() )
        trackEdit->setFocus();
//     else
//         saveTags( true );
}
// 
void mttMainWin::slotAlbumEnter()
{
    if ( yearEdit->isEnabled() )
        yearEdit->setFocus();
    else if ( genreEdit->isEnabled() )
        genreEdit->setFocus();
    else if ( commentEdit->isEnabled() )
        commentEdit->setFocus();
    else if ( trackEdit->isEnabled() )
        trackEdit->setFocus();
//     else
//         saveTags( true );
}

void mttMainWin::slotYearEnter()
{
    if ( genreEdit->isEnabled() )
        genreEdit->setFocus();
    else if ( commentEdit->isEnabled() )
        commentEdit->setFocus();
    else if ( trackEdit->isEnabled() )
        trackEdit->setFocus();
//     else
//         saveTags( true );
}
 
// void mttMainWin::slotGenreEnter()
// {
//     if ( GenCommentCLE->isEnabled() )
//         GenCommentCLE->setFocus();
//     else if ( GenTrackCLE->isEnabled() )
//         GenTrackCLE->setFocus();
//     else
//         saveTags( true );
// }

void mttMainWin::slotCommentEnter()
{
    if ( trackEdit->isEnabled() )
        trackEdit->setFocus();
//     else
//         saveTags( true );
}

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

void mttMainWin::slotSelectionChange( const QModelIndex &current, const QModelIndex &previous )
{
	mttFile *mf;
	TagLib::AudioProperties *ap;

	ignoreChange = true;
	//qDebug("slotSelectionChange");
	if ( current.isValid() ) { //FIXME: Remove the last empty row from the treeView
		titleEdit->setText( current.model()->data( current.sibling( current.row(), 1 ) ).toString() );
		artistEdit->setText( current.model()->data( current.sibling( current.row(), 2 ) ).toString() );
		albumEdit->setText( current.model()->data( current.sibling( current.row(), 3 ) ).toString() );
		yearEdit->setText( current.model()->data( current.sibling( current.row(), 4 ) ).toString() );
		genreEdit->setEditText( current.model()->data( current.sibling( current.row(), 5 ) ).toString() );
		commentEdit->setText( current.model()->data( current.sibling( current.row(), 6 ) ).toString() );
		trackEdit->setText( current.model()->data( current.sibling( current.row(), 7 ) ).toString() );
		mf = new mttFile();
		if ( current.parent().internalPointer() != NULL ) {
			QString fn = current.model()->data( current.parent().sibling( 0, 0 ) ).toString() + "/" + current.model()->data( current.sibling( current.row(), 0 ) ).toString();
			mf->Open( fn );
			if ( mf ) {
				ap = mf->getAudioProperties();
				if ( ap ) {
					int hours, min, sec;
					QString length;
	
					// Calculate track length
					hours = ap->length() / 3600;
					min = ( ap->length() - hours * 3600 ) / 60;
					sec = ( ap->length() - hours * 3600 ) % 60;
					if ( hours )
						length.sprintf( "%dh %dm %ds", hours, min, sec );
					else
						length.sprintf( "%dm %ds", min, sec );
	
					lengthLabel->setText( length );
					bitrateLabel->setText( QString::number( ap->bitrate() ) );
					sampleRateLabel->setText( QString::number( ap->sampleRate() ) );
					channelLabel->setText( QString::number( ap->channels() ) );
				}
				else {
					lengthLabel->setText( QString( "" ) );
					bitrateLabel->setText( QString( "" ) );
					sampleRateLabel->setText( QString( "" ) );
					channelLabel->setText( QString( "" ) );
				}
			}
		}
		delete mf;
	}
	ignoreChange = false;
}

void mttMainWin::slotSelectionChange( const QItemSelection &current, const QItemSelection &previous )
{
// 	renumModel.setData( current.indexes() ); //BUG? I doesn't return all the selected items. It leaves out the first of a set.
	tableView->resizeColumnsToContents();
}

void mttMainWin::slotTitleChanged( const QString &title )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 1 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), title );
		}
	}
}

void mttMainWin::slotTrackChanged( const QString &track )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 7 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), track );
		}
	}
}

void mttMainWin::slotCommentChanged( const QString &comment )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 6 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), comment );
		}
	}
}

void mttMainWin::slotYearChanged( const QString &year )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 4 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), year );
		}
	}
}

void mttMainWin::slotAlbumChanged( const QString &album )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 3 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), album );
		}
	}
}

void mttMainWin::slotArtistChanged( const QString &artist )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list += treeView->selectionModel()->selectedRows( 2 );
	
		for (int i=0;i<list.count();i++) {
	/*		ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), artist );
		}
	}
}

void mttMainWin::slotGenreChanged( const QString &genre )
{
    QModelIndexList list;
//     TreeItem *ti;

	if ( !ignoreChange ) {
		//qDebug("slotTitleChanged");
		list = treeView->selectionModel()->selectedRows( 5 );

		for (int i=0;i<list.count();i++) {
/*			ti = (TreeItem *) list.at(i).internalPointer();
			( (mttFile *) ti->getFile() )->checkEncodings();
			// Save info from the various text fields
			( (TreeItem *) list.at(i).internalPointer() )->setItemChanged( true );*/
			treeModel.setData( list.at(i), genre );
		}
	}
}
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
void mttMainWin::slotRemoveFiles()
{
/*    QList<QModelIndex> list;
    TreeItem *ti;
    int i;

    list = treeView->selectionModel()->selectedIndexes();
    for ( i = 0; i < list.size(); i+=8 ) { // 8 is the number of columns in TreeView
        treeModel.removeRows( list.at(i).row(), 1, list.at(i).parent() );
    }

    //Remove parents with no children
    for ( i = 0; i<treeModel.rowCount(); i++ ) {
        ti = (TreeItem*) treeModel.index( i, 0 ).internalPointer();
        if ( ti->childCount() == 0 )
            treeModel.removeRows( i, 1 );
    }*/
}

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
