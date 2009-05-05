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

#include <QFileDialog>
#include <QPixmap>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QCursor>
#include <QLineEdit>
#include <QLabel>
#include <QList>
#include <QFrame>
#include <QFormLayout>
#include <QPushButton>
#include <QTime>
#include <QStatusBar>
#include <QApplication>
#include <QStackedLayout>

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
//     setupUi( this );
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

    createActions();
    createMenus();

    for ( i=0; i<5; i++ ) {
        separators << " - ";
    }
#ifndef RELEASE
    setWindowTitle( windowTitle() + " - " + RV_SNAPSHOT_VERSION );
#endif

    // Initialization of TreeView
    treeView = new mttTreeView( this );
    setCentralWidget( treeView );
    treeView->setModel( &treeModel );
    treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    treeView->setSelectionBehavior( QAbstractItemView::SelectRows );
    treeView->setRootIsDecorated( true );
    //treeView->setItemDelegate( (QAbstractItemDelegate*) new mttItemDelegate() );
    treeView->setUniformRowHeights( true ); // Helps improve speed with large lists
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
	QFrame *detailFrame;
	QFormLayout *detailLayout;

	dockDetails = new QDockWidget( tr("Details"), this );
    menuView->addAction( dockDetails->toggleViewAction() );
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
	QFrame *editFrame;
	QFormLayout *formLayout;
	dockEdit = new QDockWidget( tr("Edit"), this );
    menuView->addAction( dockEdit->toggleViewAction() );
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
	//tabifyDockWidget( dockDetails, dockEdit );

	// Renumber dock
	dockRenum = new QDockWidget( tr("Renumber"), this );
    menuView->addAction( dockRenum->toggleViewAction() );
	renumModel.setSupportedDragActions( Qt::MoveAction );

	QWidget *miniwin = new QWidget( this );

	listView = new QListView( this );
	listView->setModel( &renumModel );
	listView->setAlternatingRowColors( true );
	listView->setSelectionMode( QAbstractItemView::SingleSelection );
	listView->setDragEnabled( true );
	listView->viewport()->setAcceptDrops( true );
	listView->setDropIndicatorShown( true );
	listView->setDragDropMode( QAbstractItemView::InternalMove );
	listView->setSelectionBehavior( QAbstractItemView::SelectRows );

	QPushButton *upButton = new QPushButton( tr( "Up" ) );
	QPushButton *downButton = new QPushButton( tr( "Down" ) );
	QPushButton *renumButton = new QPushButton( tr( "Renumber" ) );
	QGridLayout *layout = new QGridLayout;
    upButton->setAutoRepeat( true );
    upButton->setAutoRepeatDelay( 600 );
    downButton->setAutoRepeat( true );
    downButton->setAutoRepeatDelay( 600 );
	listView->setToolTip( QString( "<b>Track renumber:</b>\n<ol><li>Select the tracks from the main view<li>Use the up & down buttons or drag & drop to change track position<li>Press the renumber button.</ol>") );
	layout->addWidget( listView, 0, 0, 4, 2 );
	layout->addWidget( upButton, 1, 2 );
	layout->addWidget( downButton, 2, 2 );
	layout->addWidget( renumButton, 4, 0, 1, 2 );
	miniwin->setLayout( layout );
	dockRenum->setWidget( miniwin );
	this->addDockWidget( Qt::RightDockWidgetArea, dockRenum );
	tabifyDockWidget( dockRenum, dockDetails );

    // Filename format dock
    QFrame *formatFrame, *previewTagFrame, *previewFnameFrame;
    QGridLayout *formatLayout;
    QStackedLayout *stackedLayout;
    QFormLayout *previewTagLayout;
    QFrame *pFrame;
    QBoxLayout *previewFnameLayout;

    dockFormat = new QDockWidget( tr("Filename Format"), this );
    menuView->addAction( dockFormat->toggleViewAction() );
    formatFrame = new QFrame( dockFormat );
    formatLayout = new QGridLayout( formatFrame );
    stackedLayout = new QStackedLayout( formatFrame );
    previewTagFrame = new QFrame( formatFrame );
    previewTagLayout = new QFormLayout( previewTagFrame );
    titleLabel = new QLabel( previewTagFrame );
    artistLabel = new QLabel( previewTagFrame );
    albumLabel = new QLabel( previewTagFrame );
    yearLabel = new QLabel( previewTagFrame );
    genreLabel = new QLabel( previewTagFrame );
    commentLabel = new QLabel( previewTagFrame );
    trackLabel = new QLabel( previewTagFrame );

    formatType = new QComboBox( formatFrame );
    formatType->addItem( QString( tr( "Filename to Tag" ) ) );
    formatType->addItem( QString( tr( "Tag to Filename" ) ) );
    formatLayout->addWidget( formatType, 0, 0, 1, 5 );
    format = new QComboBox( formatFrame );
    format->setDuplicatesEnabled( false );
    format->setEditable( true );
    format->lineEdit()->setText( QString( "%a - %tr - %t" ) );
    legendButton = new QPushButton( tr( "Legend" ), dockFormat );
    legendButton->setCheckable( true );
    legendButton->setChecked( true );
    formatLayout->addWidget( new QLabel( tr("Format:") ), 1, 0 );
    formatLayout->addWidget( format, 1, 1, 1, 3 );
    formatLayout->addWidget( legendButton, 1, 4 );
    autoUpd = new QCheckBox( tr("Auto Update Preview"), formatFrame );
    autoUpd->setToolTip( tr( "Click to enable automatic update of the preview" ) );
    formatLayout->addWidget( autoUpd, 2, 0, 1, 3 );
    updatePreviewButton = new QPushButton( tr( "Update Preview" ) , previewTagFrame );
    updatePreviewButton->setToolTip( tr( "Press to manually update preview" ) );
    formatLayout->addWidget( updatePreviewButton, 2, 3, 1, 2 );
    formatLayout->addWidget( new QLabel( tr("<b><u>Preview</u></b>") ), 3, 0 );

    pFrame = new QFrame( formatFrame );

    previewTagLayout->addRow( QString( tr("Title:" ) ), titleLabel );
    previewTagLayout->addRow( QString( tr("Artist:") ), artistLabel );
    previewTagLayout->addRow( QString( tr("Album:") ), albumLabel );
    previewTagLayout->addRow( QString( tr("Year:") ), yearLabel );
    previewTagLayout->addRow( QString( tr("Genre:") ), genreLabel );
    previewTagLayout->addRow( QString( tr("Comment:") ), commentLabel );
    previewTagLayout->addRow( QString( tr("Track:") ), trackLabel );
    previewTagFrame->setLayout( previewTagLayout );
    previewTagFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    previewFnameLayout = new QBoxLayout( QBoxLayout::TopToBottom, formatFrame );
    previewFnameFrame = new QFrame( formatFrame );

    previewFnameLayout->addWidget( new QLabel( tr( "<u>Current</u>" ) ) );
    curfnameLabel = new QLabel( formatFrame );
    curfnameLabel->setText( tr( "Test filename" ) );
    previewFnameLayout->addWidget( curfnameLabel );
    previewFnameLayout->addWidget( new QLabel( tr( "<u>New</u>" ) ) );
    newfnameLabel = new QLabel( formatFrame );
    //newfnameLabel->setStyleSheet( QString( "border: 2px solid" ) );
    newfnameLabel->setText( tr( "New Filename" ) );
    previewFnameLayout->addWidget( newfnameLabel );

    previewFnameFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    previewFnameFrame->setLayout( previewFnameLayout );

    stackedLayout->addWidget( previewTagFrame );
    stackedLayout->addWidget( previewFnameFrame );

    pFrame->setLayout( stackedLayout );

    formatLayout->addWidget( pFrame, 4, 0, 7, 5 );

    formatFrame->setLayout( formatLayout );
    dockFormat->setWidget( formatFrame );
    this->addDockWidget( Qt::RightDockWidgetArea, dockFormat );
    tabifyDockWidget( dockFormat, dockEdit );

    // Filename Format Legend Dock-
    QFrame *formatLegendFrame;
    QFormLayout *formatLegendLayout;

    dockFormatLegend = new QDockWidget( tr( "Format Legend" ), this );
    menuView->addAction( dockFormatLegend->toggleViewAction() );
    formatLegendFrame = new QFrame( dockFormatLegend );
    formatLegendLayout = new QFormLayout( formatLegendFrame );
    formatLegendLayout->addRow( new QLabel( tr( "%t - Title" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%a - Artist" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%A - Album" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%y - Year" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%g - Genre" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%c - Comment" ), dockFormatLegend ) );
    formatLegendLayout->addRow( new QLabel( tr( "%T - Track number" ), dockFormatLegend ) );

    formatLegendFrame->setLayout( formatLegendLayout );
    dockFormatLegend->setWidget( formatLegendFrame );
    this->addDockWidget( Qt::LeftDockWidgetArea, dockFormatLegend );

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

    connect( upButton, SIGNAL( pressed() ), this, SLOT( slotUpButtonClicked() ) );
    connect( downButton, SIGNAL( pressed() ), this, SLOT( slotDownButtonClicked() ) );
    connect( renumButton, SIGNAL( clicked(bool) ), this, SLOT( slotRenumButtonClicked(bool) ) );

    connect( format, SIGNAL( editTextChanged( const QString& ) ), this, SLOT( slotFormatChanged( const QString& ) ) );
    connect( autoUpd, SIGNAL( stateChanged( int ) ), this, SLOT( slotAutoPreviewChanged( int ) ) );
    connect( autoUpd, SIGNAL( toggled( bool ) ), this, SLOT( slotFormatEnableAutoUpdate( bool ) ) );
    connect( legendButton, SIGNAL( clicked( bool ) ), this, SLOT( slotLegendToggle( bool ) ) );
    connect( dockFormatLegend, SIGNAL( visibilityChanged( bool ) ), this, SLOT( slotDockFormatLegendChanged( bool ) ) );
    connect( updatePreviewButton, SIGNAL( clicked( bool ) ), this, SLOT( slotFormatUpdatePreview() ) );
    connect( format->lineEdit(), SIGNAL( textEdited( const QString& ) ), this, SLOT( slotFormatEdited() ) );

    connect( formatType, SIGNAL( activated( int ) ), stackedLayout, SLOT( setCurrentIndex( int ) ) );

    // Signal & Slot connections for menubar
    connect( actExit, SIGNAL( triggered() ), this, SLOT( close() ) );
    connect( actOpenFolder, SIGNAL( triggered() ), this, SLOT( slotOpen() ) );
    connect( actOpenFiles, SIGNAL( triggered() ), this, SLOT( slotOpenFiles() ) );

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

	//qDebug( fname.toUtf8().constData() );
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
    if ( itemChanged ) {
        QFont f;
        f.setBold( true );
        f.setUnderline( true );
		newrow[0]->setFont( f );
    }
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

void mttMainWin::slotRemoveTags()
{
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
//             progress.hide();
//             statusBar()->message( tr( QString( "Done" ) ) );
//             QApplication::restoreOverrideCursor();
//         }
//     }
}

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
    int i;

    list = treeView->selectionModel()->selectedIndexes();

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

//     ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
    titleEdit->setText( titleEdit->text().toUpper() );
//     }
//     if( GenArtistChkB->isChecked() ) {
    artistEdit->setText( artistEdit->text().toUpper() );
//     }
//     if( GenAlbumChkB->isChecked() ) {
    albumEdit->setText( albumEdit->text().toUpper() );
//     }
//     if( GenCommentChkB->isChecked() ) {
    commentEdit->setText( commentEdit->text().toUpper() );
//     }
    genreEdit->lineEdit()->setText( genreEdit->lineEdit()->text().toUpper() );
    ignoreChange = false;

    for ( i = 0; i < (list.size()/8); i++ ) { // 8 is the number of columns in TreeView
        //treeModel.setData( list.at(i), treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ) );
        treeModel.setData( list.at(i + list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 2*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 3*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toUpper() );
        //treeModel.setData( list.at(i + 4*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString() );
        treeModel.setData( list.at(i + 5*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 6*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toUpper() );
        //treeModel.setData( list.at(i + 7*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString() );
        setTagChanged( list.at(i) );
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

    ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
    titleEdit->setText( titleEdit->text().toLower() );
//     }
//     if( GenArtistChkB->isChecked() ) {
    artistEdit->setText( artistEdit->text().toLower() );
//     }
//     if( GenAlbumChkB->isChecked() ) {
    albumEdit->setText( albumEdit->text().toLower() );
//     }
//     if( GenCommentChkB->isChecked() ) {
    commentEdit->setText( commentEdit->text().toLower() );
//     }
    genreEdit->lineEdit()->setText( genreEdit->lineEdit()->text().toLower() );
    ignoreChange = false;

    for ( i = 0; i < (list.size()/8); i++ ) { // 8 is the number of columns in TreeView
        //treeModel.setData( list.at(i), treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ) );
        treeModel.setData( list.at(i + list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 2*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 3*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() );
        //treeModel.setData( list.at(i + 4*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString() );
        treeModel.setData( list.at(i + 5*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 6*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() );
        //treeModel.setData( list.at(i + 7*list.size()/8), treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString() );
        setTagChanged( list.at(i) );
    }

    QApplication::restoreOverrideCursor();
}

void mttMainWin::slotFirstUpWords()
{
    QList<QModelIndex> list;

    list = treeView->selectionModel()->selectedIndexes();
    ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
        titleEdit->setText( firstUp( titleEdit->text().toLower() ) );
//     }
//     if( GenArtistChkB->isChecked() ) {
        artistEdit->setText( firstUp( artistEdit->text().toLower() ) );
//     }
//     if( GenAlbumChkB->isChecked() ) {
        albumEdit->setText( firstUp( albumEdit->text().toLower() ) );
//     }
//     if( GenCommentChkB->isChecked() ) {
        commentEdit->setText( firstUp( commentEdit->text().toLower() ) );
//     }
    ignoreChange = false;
// 
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    for ( int i = 0; i < (list.size()/8); i++ ) { // 8 is the number of columns in TreeView
        treeModel.setData( list.at(i + list.size()/8), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 2*list.size()/8), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 3*list.size()/8), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 5*list.size()/8), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 6*list.size()/8), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() ) );
        setTagChanged( list.at(i) );
    }

    QApplication::restoreOverrideCursor();
}

void mttMainWin::slotFirstUpSentence() // Here is the change to get only the first letter uppercase (only track name)
{
    QList<QModelIndex> list;

    list = treeView->selectionModel()->selectedIndexes();
    ignoreChange = true; // It's needed because otherwise all the files would have this tag
//     if( GenTitleChkB->isChecked() ) {
        titleEdit->setText( firstUpSentence( titleEdit->text().toLower() ) );
//     }
//     if( GenArtistChkB->isChecked() ) {
        artistEdit->setText( firstUpSentence( artistEdit->text().toLower() ) );
//     }
//     if( GenAlbumChkB->isChecked() ) {
        albumEdit->setText( firstUpSentence( albumEdit->text().toLower() ) );
//     }
//     if( GenCommentChkB->isChecked() ) {
        commentEdit->setText( firstUpSentence( commentEdit->text().toLower() ) );
//     }
    ignoreChange = false;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    for ( int i = 0; i < (list.size()/8); i++ ) { // 8 is the number of columns in TreeView
        treeModel.setData( list.at(i + list.size()/8), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 2*list.size()/8), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 3*list.size()/8), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 5*list.size()/8), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 6*list.size()/8), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() ) );
        setTagChanged( list.at(i) );
    }

    QApplication::restoreOverrideCursor();
}


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

QString mttMainWin::firstUpSentence( QString str )
{
    if ( str[0].isLetter() ) {
        str[0] = str[0].toUpper();
    }

    return str;
}

QString mttMainWin::firstUp( QString str )
{
    unsigned int i;

    i = 0;
    while ( i < str.length() ) {
        if ( i == 0 && str[i].isLetter() ) {
            str[i] = str[i].toUpper();
        }
        if ( str[i].isSpace() && ( ( i + 1 ) < str.length() ) && str[i+1].isLetter() ) {
            str[i+1] = str[i+1].toUpper();
        }
        i++;
    }

    return str;
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

    if ( dockFormat->isVisible() && autoUpd->isChecked() ) {
        slotFormatUpdatePreview();
        slotFormatUpdateFnamePreview( current );
    }
}

void mttMainWin::slotSelectionChange( const QItemSelection &current, const QItemSelection &previous )
{
	QModelIndexList l = treeView->selectionModel()->selectedIndexes();
	renumModel.removeRows( 0, renumModel.rowCount() );
	for ( int i=(l.size()/8); i < (l.size()/8*2); i++ ) {
		QStandardItem *item = new QStandardItem( l[i].data().toString() );
        item->setData( l[i-l.size()/8].data().toString(), Qt::UserRole );
		item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled );
		renumModel.invisibleRootItem()->appendRow( item );
	}
}

void mttMainWin::slotTitleChanged( const QString &title )
{
    changeColumnText( 1, title );
}

void mttMainWin::slotArtistChanged( const QString &artist )
{
    changeColumnText( 2, artist );
}

void mttMainWin::slotAlbumChanged( const QString &album )
{
    changeColumnText( 3, album );
}

void mttMainWin::slotYearChanged( const QString &year )
{
    changeColumnText( 4, year );
}

void mttMainWin::slotGenreChanged( const QString &genre )
{
    changeColumnText( 5, genre );
}

void mttMainWin::slotCommentChanged( const QString &comment )
{
    changeColumnText( 6, comment );
}

void mttMainWin::slotTrackChanged( const QString &track )
{
    changeColumnText( 7, track );
}

void mttMainWin::changeColumnText( int column, const QString &text )
{
    QModelIndexList list;

	if ( !ignoreChange ) {
		list = treeView->selectionModel()->selectedRows( column ); // Selected items that we want to change
	
		for (int i=0;i<list.count();i++) {
			treeModel.setData( list.at(i), text );
            setTagChanged( list.at(i) );
		}
	}
}
// 
// void mttMainWin::saveTags( bool selectedOnly = false )
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
    QList<QModelIndex> list;
    QStandardItem *ti;
    int i;

    // A not so speedy method but surely an efficient one!
    list = treeView->selectionModel()->selectedIndexes();
    while ( !list.empty() ) {
        treeModel.removeRows( list.at(0).row(), 1, list.at(0).parent() );
        list = treeView->selectionModel()->selectedIndexes();
    }

    //Remove parents with no children
    for ( i = 0; i<treeModel.rowCount(); i++ ) {
        if ( !treeModel.hasChildren( treeModel.index( i, 0 ) ) )
            treeModel.removeRows( i, 1 );
    }
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

void mttMainWin::slotUpButtonClicked()
{
    QModelIndexList l;
    QModelIndex mi;
    QVariant tmp;

    l = listView->selectionModel()->selectedIndexes();

    if ( !l.empty() ) {
        // There can be only one selected item so we use l[0] without any other checks
        if ( l[0].row() ) {
            mi = l[0].sibling( l[0].row() - 1, l[0].column() );
            tmp = l[0].data( Qt::DisplayRole );
            renumModel.setData( l[0], mi.data( Qt::DisplayRole ), Qt::DisplayRole );
            renumModel.setData( mi, tmp, Qt::DisplayRole );
            listView->selectionModel()->setCurrentIndex( mi, QItemSelectionModel::ClearAndSelect );
        }
    }
}

void mttMainWin::slotDownButtonClicked()
{
    QModelIndexList l;
    QModelIndex mi;
    QVariant tmp;

    l = listView->selectionModel()->selectedIndexes();

    if ( !l.empty() ) {
        // There can be only one selected item so we use l[0] without any other checks
        if ( l[0].row() < ( renumModel.rowCount() - 1 ) ) {
            mi = l[0].sibling( l[0].row() + 1, l[0].column() );
            tmp = l[0].data( Qt::DisplayRole );
            renumModel.setData( l[0], mi.data( Qt::DisplayRole ), Qt::DisplayRole );
            renumModel.setData( mi, tmp, Qt::DisplayRole );
            listView->selectionModel()->setCurrentIndex( mi, QItemSelectionModel::ClearAndSelect );
        }
    }
}

void mttMainWin::slotRenumButtonClicked( bool checked )
{
    QModelIndexList l;
    QList<QStandardItem*> li;
    int items;

    l = treeView->selectionModel()->selectedIndexes();

    for( int i=0; i<renumModel.rowCount(); i++ ) {
        li = renumModel.findItems( l.at(l.count()/8+i).data().toString() ); // Take renum model item that matches the filename of the selected treemodel item
        treeModel.setData( l.at( 7*l.count()/8 + i ), QVariant( li[0]->row() + 1 ) );
        qDebug( renumModel.data( renumModel.index( i, 0 ), Qt::UserRole ).toString().toUtf8().constData() );
    }
}

void mttMainWin::setTagChanged( const QModelIndex &index, bool changed )
{
    treeModel.setData( index.sibling( index.row(), 0 ) , changed, Qt::UserRole ); // Modify changed flag
    if ( changed ) {
/*        QFont f;
        f.setBold( true );
        f.setUnderline( true );
        treeModel.setData( index.sibling( index.row(), 0 ), f, Qt::FontRole );*/
        for ( int i=0; i<8; i++ )
            treeModel.setData( index.sibling( index.row(), i ), Qt::red, Qt::BackgroundRole );
    }
    else {
/*        QFont f;
        f.setBold( false );
        f.setUnderline( false );
        treeModel.setData( index.sibling( index.row(), 0 ), f, Qt::FontRole );*/
        for ( int i=0; i<8; i++ )
            treeModel.setData( index.sibling( index.row(), i ), QVariant(), Qt::BackgroundRole );
    }
}

void mttMainWin::slotFormatChanged( const QString &str )
{
    qDebug( "Format Changed!" );
}

void mttMainWin::slotAutoPreviewChanged( int state )
{
    if ( state == Qt::Unchecked ) {
        updatePreviewButton->setEnabled( true );
        updatePreviewButton->setToolTip( tr( "Press to manually update preview" ) );
    }
    else {
        updatePreviewButton->setEnabled( false );
        updatePreviewButton->setToolTip( tr( "Press to manually update preview<br><br><b>Currently disabled!</b><br>Uncheck 'Automatic Update' to enable use of this button" ) );
    }
}

void mttMainWin::slotLegendToggle( bool checked )
{
    QAction *a;

    if ( dockFormatLegend ) {
        a = dockFormatLegend->toggleViewAction();
        if ( a->isChecked() )
            removeDockWidget( dockFormatLegend );
        else {
            addDockWidget( Qt::LeftDockWidgetArea, dockFormatLegend );
            dockFormatLegend->show();
        }
    }
}

void mttMainWin::createActions( void )
{
    actOpenFolder = new QAction( tr( "&Open Folder" ), this );
    actOpenFolder->setShortcut( tr( "Ctrl+O" ) );
    actOpenFiles = new QAction( tr( "Open &Files" ), this );
    actExit = new QAction( tr( "E&xit" ), this );
    actExit->setShortcut( tr( "Alt+X" ) );
    actSaveAll = new QAction( tr( "&Save all changes" ), this );
    actSaveSelected = new QAction( tr( "Save selected tags" ), this );
    actRemoveTag = new QAction( tr( "Remove selected tags" ), this );
    actHelp = new QAction( tr( "Help" ), this );
    actAbout = new QAction( tr( "About" ), this );
}

void mttMainWin::createMenus( void )
{
    menuFile = menuBar()->addMenu( tr( "&File" ) );
    menuFile->addAction( actOpenFolder );
    menuFile->addAction( actOpenFiles );
    menuFile->addSeparator();
    menuFile->addAction( actExit );

    menuTag = menuBar()->addMenu( tr( "&Tag" ) );
    menuTag->addAction( actSaveAll );
    menuTag->addAction( actSaveSelected );
    menuTag->addSeparator();
    menuTag->addAction( actRemoveTag );

    menuView = menuBar()->addMenu( tr( "&View" ) );

    menuHelp = menuBar()->addMenu( tr( "&Help" ) );
    menuHelp->addAction( actHelp );
    menuHelp->addSeparator();
    menuHelp->addAction( actAbout );
}

void mttMainWin::slotDockFormatLegendChanged( bool visible )
{
    if ( visible && !legendButton->isChecked() )
        legendButton->setChecked( true );
    if ( !visible && legendButton->isChecked() )
        legendButton->setChecked( false );
}

void mttMainWin::slotFormatEnableAutoUpdate( bool enabled )
{
    slotFormatUpdatePreview();
}

void mttMainWin::slotFormatUpdatePreview( void )
{
    if ( formatType->currentIndex() == 0 ) { // Filename to Tag
        int i;
        QString filename;
        QStringList strList;

        filename = ( treeView->currentIndex().sibling( treeView->currentIndex().row(), 0 ) ).data( Qt::DisplayRole ).toString();

        // Remove the extension from the filename
        for ( i = ( filename.length() - 1 ); i >= 0; i-- )
            if ( filename[i] == '.' )
                break;
        if ( i )
            filename = filename.left( i );

        strList = getTagsFromFilename( filename );

        artistLabel->setText( strList.at( 0 ) );
        albumLabel->setText( strList.at( 1 ) );
        titleLabel->setText( strList.at( 2 ) );
        trackLabel->setText( strList.at( 3 ) );
        commentLabel->setText( strList.at( 4 ) );
        yearLabel->setText( strList.at( 5 ) );
        genreLabel->setText( strList.at( 6 ) );
    }
    else { // Tag to filename
    }
}

QStringList mttMainWin::getTagsFromFilename( QString filename )
{
        int idx = 0, offset = 0, previdx = 0;
        QRegExp rx("%[aAtTcyg]\\b");
        QString str;
        QList<int> idxList;
        QList<QString> strList;
        bool done = false;
        QString artist, album, track, title, year, comment, genre;

        // Find the format fields in the format string
        while ( ( idx = rx.indexIn( format->currentText(), offset ) ) != -1) {
            idxList << idx;
            str = rx.cap();
            strList << str;
            offset = idx + str.length();
            //qDebug( QString::number( idx ).toUtf8().constData() );
            //qDebug( QString::number( offset ).toUtf8().constData() );
            //QMessageBox::information( this, "Match", str );
        }

        // Match the fields with part of the filename
        for ( int i = 0; ( i < idxList.count() ) && !done; i++ ) {
            QString seperator;

            // Create the seperator
            for ( int j = idxList.at( i ) + 2; j < ( ( i == ( idxList.count() - 1 ) ) ? format->currentText().length() : idxList.at( i + 1 ) ); j++ ) {
                seperator[ j - idxList.at( i ) - 2 ] = format->currentText()[j];
            }

            //QMessageBox::information( this, "Seperator", seperator );
            if ( ( ( seperator.length() == 0 ) && ( i != ( idxList.count() - 1 ) ) ) || ( ( idx = filename.indexOf( seperator, previdx, Qt::CaseSensitive ) ) == -1 ) )
                done = true; // There is no reason to try to match the rest
            else {
                if ( idx == previdx ) // The only reason for this to happen is that we are at the last field of the format
                    idx = filename.length();

                QString s;
                QString f = strList.at(i);

                for ( int k = previdx; k < idx; k++ )
                    s[ k - previdx ] = filename[k];

                if ( f == "%a" )
                    artist = s ;
                else if ( f == "%A" )
                    album = s;
                else if ( f == "%t" )
                    title = s;
                else if ( f == "%T" )
                    track = s;
                else if ( f == "%c" )
                    comment = s;
                else if ( f == "%y" )
                    year = s;
                else if ( f == "%g" )
                    genre = s;
                else
                    QMessageBox::critical( this, tr( "Invalid format field!" ), tr( "Invalid format field " ) + f + tr( ". Please check the format you typed." ) );
            }
            previdx = idx + seperator.length();
        }

        QStringList l;
        l << artist << album << title << track << comment << year << genre;
        return l;
}

void mttMainWin::slotFormatEdited()
{
    if ( dockFormat->isVisible() && autoUpd->isChecked() )
        slotFormatUpdatePreview();
}

void mttMainWin::slotFormatUpdateFnamePreview( const QModelIndex &current )
{
    QString curfname, newfname, cformat;
    bool done = false;

    curfname = current.model()->data( current.sibling( current.row(), 0 ) ).toString();
    curfnameLabel->setText( curfname );

    cformat = format->currentText();
    while ( !done ) {
        if ( cformat.startsWith( "%a" ) )
            newfname += current.model()->data( current.sibling( current.row(), 2 ) ).toString();
        if ( cformat.startsWith( "%A" ) )
            newfname += current.model()->data( current.sibling( current.row(), 3 ) ).toString();
        if ( cformat.startsWith( "%t" ) )
            newfname += current.model()->data( current.sibling( current.row(), 1 ) ).toString();
        if ( cformat.startsWith( "%T" ) ) {
            if ( current.model()->data( current.sibling( current.row(), 7 ) ).toInt() < 10 )
                newfname += '0';
            newfname += current.model()->data( current.sibling( current.row(), 7 ) ).toString();
        }
        if ( cformat.startsWith( "%y" ) )
            newfname += current.model()->data( current.sibling( current.row(), 4 ) ).toString();
        if ( cformat.startsWith( "%g" ) )
            newfname += current.model()->data( current.sibling( current.row(), 5 ) ).toString();
        if ( cformat.startsWith( "%c" ) )
            newfname += current.model()->data( current.sibling( current.row(), 6 ) ).toString();
        cformat.remove( 0, 2 );

        if ( cformat == "" )
            done = true;
        else {
            newfname += cformat.left( cformat.indexOf( "%" ) );
            cformat.remove( 0, cformat.indexOf( "%" ) );
            if ( cformat == "" )
                done = true;
        }
    }
    newfname += curfname.right( curfname.length() - curfname.lastIndexOf( "." ) );

    newfnameLabel->setText( newfname );
}
