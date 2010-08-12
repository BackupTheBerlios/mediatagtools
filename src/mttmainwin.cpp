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
#include <QtDebug>
#include <QSettings>
#include <QPointer>

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
#include "ui_about.h"
#include "mttaboutdialog.h"
#include "mttmainwin.h"
#include "mttitemdelegate.h"
#include "mttfile.h"
//#include "mttcfdialog.h"
//#include "mttaboutdialog.h"
#include "config.h"
//#include "x.xpm"
#include "tools.h"
#include "mttadvtagdelegate.h"

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
    setWindowTitle( QString("MediaTagTools") + " - " + RV_SNAPSHOT_VERSION );
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
    treeView->setSortingEnabled( true );
    treeView->show();

    QStringList header;
    header << tr( "Filename" ) << tr( "Title" ) << tr( "Artist" ) << tr( "Album" )
           << tr( "Year" ) << tr( "Genre" ) << tr( "Comment" ) << tr( "Track" ) << tr("File Type");
    treeModel.setHorizontalHeaderLabels( header );

    // Signal & Slot connections for TreeView
    connect( treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotSelectionChange(const QModelIndex&, const QModelIndex&)) );
    connect( treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(slotSelectionChange(const QItemSelection&, const QItemSelection&)) );
    connect( &treeModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(slotItemChanged(QStandardItem *)));

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
    detailLayout->addRow( tr("<b>Length:</b>"), lengthLabel );
    bitrateLabel = new QLabel();
    detailLayout->addRow( tr("<b>Bitrate:</b>"), bitrateLabel );
    sampleRateLabel = new QLabel();
    detailLayout->addRow( tr("<b>Sample Rate:</b>"), sampleRateLabel );
    channelLabel = new QLabel();
    detailLayout->addRow( tr("<b>Channels:</b>"), channelLabel );
    detailFrame->setLayout( detailLayout );
    dockDetails->setWidget( detailFrame );
    dockDetails->setObjectName(QString("Details"));
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
    dockEdit->setObjectName(QString("Edit"));
    this->addDockWidget( Qt::RightDockWidgetArea, dockEdit );

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
    dockRenum->setObjectName(QString("Renumber"));
    this->addDockWidget( Qt::RightDockWidgetArea, dockRenum );

    // Filename format dock
    QFrame *formatFrame, *previewTagFrame, *previewFnameFrame;
    QGridLayout *formatLayout;
    QStackedLayout *stackedLayout;
    QFormLayout *previewTagLayout;
    QFrame *pFrame;
    //QBoxLayout *previewFnameLayout;
    QFrame *applyButtonF2TFrame, *applyButtonT2FFrame;
    QBoxLayout *applyButtonF2TLayout, *applyButtonT2FLayout;
    QPushButton *applyF2T, *applyT2F;
    QFormLayout *previewFnameLayout;

    dockFormat = new QDockWidget( tr("Filename Format"), this );
    menuView->addAction( dockFormat->toggleViewAction() );
    formatFrame = new QFrame( dockFormat );
    formatLayout = new QGridLayout();
    stackedLayout = new QStackedLayout();
    previewTagFrame = new QFrame( formatFrame );
    previewTagLayout = new QFormLayout();
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
    format->lineEdit()->setText( QString( "%T - %a - %t" ) );
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

    applyButtonF2TLayout = new QBoxLayout( QBoxLayout::LeftToRight, formatFrame );
    applyButtonF2TLayout->addStretch();
    applyF2T = new QPushButton(QString("Apply"));
    applyButtonF2TLayout->addWidget( applyF2T );
    applyButtonF2TFrame = new QFrame( formatFrame );
    applyButtonF2TFrame->setLayout(applyButtonF2TLayout);

    previewTagLayout->addRow( QString( tr("<b>Title:</b>" ) ), titleLabel );
    previewTagLayout->addRow( QString( tr("<b>Artist:</b>") ), artistLabel );
    previewTagLayout->addRow( QString( tr("<b>Album:</b>") ), albumLabel );
    previewTagLayout->addRow( QString( tr("<b>Year:</b>") ), yearLabel );
    previewTagLayout->addRow( QString( tr("<b>Genre:</b>") ), genreLabel );
    previewTagLayout->addRow( QString( tr("<b>Comment:</b>") ), commentLabel );
    previewTagLayout->addRow( QString( tr("<b>Track:</b>") ), trackLabel );
    previewTagLayout->addRow( applyButtonF2TFrame );
    previewTagFrame->setLayout( previewTagLayout );
    previewTagFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    //previewFnameLayout = new QBoxLayout( QBoxLayout::TopToBottom, formatFrame );
    previewFnameLayout = new QFormLayout();
    previewFnameFrame = new QFrame( formatFrame );
    
    applyButtonT2FLayout = new QBoxLayout( QBoxLayout::LeftToRight, formatFrame );
    applyButtonT2FLayout->addStretch();
    applyT2F = new QPushButton(QString("Apply"));
    applyButtonT2FLayout->addWidget( applyT2F );
    applyButtonT2FFrame = new QFrame( formatFrame );
    applyButtonT2FFrame->setLayout(applyButtonT2FLayout);
    
    previewFnameLayout->addWidget( new QLabel( tr( "<b><u>Current</u></b>" ) ) );
    curfnameLabel = new QLabel( formatFrame );
    curfnameLabel->setText( "" );
    previewFnameLayout->addWidget( curfnameLabel );
    previewFnameLayout->addWidget( new QLabel( tr( "<b><u>New</u></b>" ) ) );
    newfnameLabel = new QLabel( formatFrame );
    //newfnameLabel->setStyleSheet( QString( "border: 2px solid" ) );
    newfnameLabel->setText( "" );
    previewFnameLayout->addWidget( newfnameLabel );
    previewFnameLayout->addWidget( applyButtonT2FFrame );
    

    previewFnameFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    previewFnameFrame->setLayout( previewFnameLayout );

    stackedLayout->addWidget( previewTagFrame );
    stackedLayout->addWidget( previewFnameFrame );

    pFrame->setLayout( stackedLayout );

    formatLayout->addWidget( pFrame, 4, 0, 7, 5 );

    formatFrame->setLayout( formatLayout );
    dockFormat->setWidget( formatFrame );
    dockFormat->setObjectName(QString("Filename Format"));
    this->addDockWidget( Qt::RightDockWidgetArea, dockFormat );

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
    dockFormatLegend->setObjectName(QString("Format Legend"));
    this->addDockWidget( Qt::LeftDockWidgetArea, dockFormatLegend );

    // Advanced Tag Dock
    QFrame *advFrame;
    //QLayout *advLayout;
    QStringList strl;
    QGridLayout *advLayout;

    dockAdvancedTags = new QDockWidget( tr( "Advanced Tags" ), this );
    dockAdvancedTags->setObjectName(QString("Advanced Tags"));
    advFrame = new QFrame( dockAdvancedTags );
    advFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    advTable = new mttTableWidget(advFrame);
    advTable->setColumnCount(2);
    advTable->insertRow(0);
    strl << tr("Tag") << tr("Value");
    advTable->setHorizontalHeaderLabels(strl);
    advTable->horizontalHeader()->setStretchLastSection(true);
    advTable->verticalHeader()->setVisible(false);
    //advTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    advLayout = new QGridLayout(advFrame);
    advLayout->addWidget(advTable);
    advTable->setItemDelegate(new mttAdvTagDelegate());

    advFrame->setLayout(advLayout);
    dockAdvancedTags->setWidget(advFrame);
    this->addDockWidget( Qt::RightDockWidgetArea, dockAdvancedTags);

    // Tabify docks for first time
    tabifyDockWidget( dockRenum, dockFormat );
    tabifyDockWidget( dockRenum, dockDetails );
    tabifyDockWidget( dockAdvancedTags, dockEdit );
    setTabPosition( Qt::RightDockWidgetArea, QTabWidget::West );

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
    connect( applyF2T, SIGNAL( clicked( bool ) ), this, SLOT( slotFormatApplyToTags() ) );
    connect( applyT2F, SIGNAL( clicked( bool ) ), this, SLOT( slotFormatApplyToFilenames() ) );

    connect( formatType, SIGNAL( activated( int ) ), stackedLayout, SLOT( setCurrentIndex( int ) ) );
    connect( formatType, SIGNAL( activated( int ) ), this, SLOT( slotFormatUpdatePreview() ) );

    // Signal & Slot connections for menubar
    connect( actExit, SIGNAL( triggered() ), this, SLOT( close() ) );
    connect( actOpenFolder, SIGNAL( triggered() ), this, SLOT( slotOpen() ) );
    connect( actOpenFiles, SIGNAL( triggered() ), this, SLOT( slotOpenFiles() ) );
    connect( actHelp, SIGNAL( triggered() ), this, SLOT( slotNotImplemented() ) );
    connect( actAboutQt, SIGNAL( triggered() ), this, SLOT( slotAboutQt() ) );
    connect( actAbout, SIGNAL( triggered() ), this, SLOT( slotAbout() ) );
    connect( actRemoveTag, SIGNAL( triggered() ), this, SLOT( slotRemoveTags() ) );
    connect( actSaveAll, SIGNAL( triggered() ), this, SLOT( slotNotImplemented() ) );
    connect( actSaveSelected, SIGNAL( triggered() ), this, SLOT( slotNotImplemented() ) );

    connect( advTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT( slotEditAdvTagTableItem(QTableWidgetItem*)));

    // Create list for taglib known filetypes
    TagLib::StringList tl = TagLib::FileRef::defaultFileExtensions();
    for ( unsigned int i=0; i<tl.size(); i++ ) {
        knownFiletypes << QString( "*." ) + TStringToQString( tl[i] );
    }

    //new ModelTest(&treeModel, this);
    readSettings();
    setWindowIcon( QIcon( QString(":/icons/mediatagtools.svg") ) );
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
    mttFile li;
    TagLib::Tag *t;
    QList<QStandardItem *> fatherlist, newrow;
    QStandardItem *father, *tmp;
    QList<QVariant> list;
    bool itemChanged;

    itemChanged = li.Open( fname );

    //qDebug( fname.toUtf8().constData() );
    // Search if another file was opened from the same path
    fatherlist = treeModel.findItems( curPath );
    // If not then create a new branch
    if ( fatherlist.empty() ) {
        father = treeModel.invisibleRootItem();
        QStandardItem *item = new QStandardItem( curPath );
	// TODO: Show artwork instead of just the audio cd icon
	item->setIcon(QIcon(QString(":/icons/media-optical-audio.png")));
        father->appendRow( item );
        father = item;
    }
    else
        father = fatherlist[0];

    QStandardItem *tmp_std_item;
    tmp_std_item = new QStandardItem( fname.right( fname.size() - curPath.size() -1 ) );
    // TODO: Show per file artwork
    //     tmp_std_item->setIcon(QIcon(QString(":/icons/media-optical-audio.png")));
    newrow << tmp_std_item;
    t = li.getTag();
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

    // TODO: If the file had id3v1 tag that was converted to id3v2 make it different from the others
//    if ( itemChanged ) {
//        QFont f;
//        f.setBold( true );
//        f.setUnderline( true );
//        newrow[0]->setFont( f );
//    }
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
    QList<QModelIndex> list;
    int i;
    QString fullpath;

    list = treeView->selectionModel()->selectedIndexes();

    ignoreChange = true;
    statusBar()->addWidget( &progress );
    progress.setRange( 0, list.size() / num_col );
    progress.setValue( 0 );
    statusBar()->showMessage( tr( "Removing tags..." ) );

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    for ( i = 0; i < (list.size()/num_col); i++ ) { // num_col is the number of columns in TreeView
        fullpath = treeModel.data( list.at(i).parent(), Qt::DisplayRole).toString() + "/" + treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ).toString();

        mttFile f;
        f.Open(fullpath);
        f.removeTag();

        //treeModel.setData( list.at(i), treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ) );
        treeModel.setData( list.at(i + list.size()/num_col), QString() );
        treeModel.setData( list.at(i + 2*list.size()/num_col), QString() );
        treeModel.setData( list.at(i + 3*list.size()/num_col), QString() );
        treeModel.setData( list.at(i + 4*list.size()/num_col), QString("0") );
        treeModel.setData( list.at(i + 5*list.size()/num_col), QString() );
        treeModel.setData( list.at(i + 6*list.size()/num_col), QString() );
        treeModel.setData( list.at(i + 7*list.size()/num_col), QString("0") );
        progress.setValue(i);
    }

    statusBar()->removeWidget( &progress );
    statusBar()->showMessage( tr( "Done" ) );
    QApplication::restoreOverrideCursor();
    ignoreChange = false;
}

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

    for ( i = 0; i < (list.size()/num_col); i++ ) { // num_col is the number of columns in TreeView
        //treeModel.setData( list.at(i), treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ) );
        treeModel.setData( list.at(i + list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 2*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 3*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toUpper() );
        //treeModel.setData( list.at(i + 4*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString() );
        treeModel.setData( list.at(i + 5*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toUpper() );
        treeModel.setData( list.at(i + 6*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toUpper() );
        //treeModel.setData( list.at(i + 7*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString() );
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

    for ( i = 0; i < (list.size()/num_col); i++ ) { // num_col is the number of columns in TreeView
        //treeModel.setData( list.at(i), treeModel.data( treeModel.index( list.at(i).row(), 0, list.at(i).parent() ), Qt::DisplayRole ) );
        treeModel.setData( list.at(i + list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 2*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 3*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() );
        //treeModel.setData( list.at(i + 4*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 4 ), Qt::DisplayRole ).toString() );
        treeModel.setData( list.at(i + 5*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() );
        treeModel.setData( list.at(i + 6*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() );
        //treeModel.setData( list.at(i + 7*list.size()/num_col), treeModel.data( list.at(i).sibling( list.at(i).row(), 7 ), Qt::DisplayRole ).toString() );
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

    for ( int i = 0; i < (list.size()/num_col); i++ ) { // num_col is the number of columns in TreeView
        treeModel.setData( list.at(i + list.size()/num_col), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 2*list.size()/num_col), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 3*list.size()/num_col), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 5*list.size()/num_col), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 6*list.size()/num_col), firstUp( treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() ) );
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
    for ( int i = 0; i < (list.size()/num_col); i++ ) { // num_col is the number of columns in TreeView
        treeModel.setData( list.at(i + list.size()/num_col), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 1 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 2*list.size()/num_col), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 2 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 3*list.size()/num_col), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 3 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 5*list.size()/num_col), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 5 ), Qt::DisplayRole ).toString().toLower() ) );
        treeModel.setData( list.at(i + 6*list.size()/num_col), firstUpSentence( treeModel.data( list.at(i).sibling( list.at(i).row(), 6 ), Qt::DisplayRole ).toString().toLower() ) );
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
	for ( int i=(l.size()/num_col); i < (l.size()/num_col*2); i++ ) {
		QStandardItem *item = new QStandardItem( l[i].data().toString() );
        item->setData( l[i-l.size()/num_col].data().toString(), Qt::UserRole );
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

void mttMainWin::saveTags( bool selectedOnly = false )
{
    QString fname, title, artist, album, year, genre, comment, track;
    int current, count;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    statusBar()->showMessage( tr( "Writing tags..." ) );
    if ( selectedOnly ) {
	QModelIndexList l;
	l = treeView->selectionModel()->selectedIndexes();
	count = l.count() / num_col;
	current = 0;
	progress.setRange(0,100);
	progress.setValue(0);
	progress.show();
	
	int i =0;
	bool abort = false;
	while ( !abort && i<count ) {
	    if ( l.at(i).parent() != treeView->rootIndex() ) { // If it is a file entry...
		fname = l.at(i).parent().data().toString() + "/" + l.at(i).data().toString();
		title = l.at(count + i).data().toString();
		artist = l.at(2*count + i).data().toString();
		album = l.at(3*count + i).data().toString();
		year = l.at(4*count + i).data().toString();
		genre = l.at(5*count + i).data().toString();
		comment = l.at(6*count + i).data().toString();
		track = l.at(7*count + i).data().toString();
		
		mttFile f;
		TagLib::Tag *t;
		bool changed = false;
		f.Open(fname);
		t = f.getTag();
		if ( isTagChanged(l.at(i)) ) {
		    t->setTitle(Q4StringToTString(title));
		    t->setArtist(Q4StringToTString(artist));
		    t->setAlbum(Q4StringToTString(album));
		    t->setYear(year.toUInt());
		    t->setGenre(Q4StringToTString(genre));
		    t->setComment(Q4StringToTString(comment));
		    t->setTrack(track.toUInt());
		    if (!f.saveTag()) {
			if ( QMessageBox::critical( this, QString( tr( "Save Error" ) ), QString("Error saving tags of file ") + fname +"!", QMessageBox::Ok, QMessageBox::Abort ) == QMessageBox::Abort ) {
			    abort = true;
			}
		    }
		}
		current++;
		progress.setValue((float) current/count);
	    }
	    i++;
	}
    }
    else {
	QModelIndex ci, ii, fi;
	fi = treeView->rootIndex();
	int children;
	children = treeModel.rowCount();
	while (children > 0) {
	    ci = fi.child(children, 0);
	    count = treeModel.rowCount(ci);
	    current = 0;
	    progress.setRange(0,100);
	    progress.setValue(0);
	    progress.show();
	    
	    for (int i=0; i<count; i++) {
		ii = ci.child(i,0);
		fname = ii.parent().data().toString() + "/" + ii.data().toString();
		title = ii.sibling(ii.row(),1).data().toString();
		artist = ii.sibling(ii.row(),2).data().toString();
		album = ii.sibling(ii.row(),3).data().toString();
		year = ii.sibling(ii.row(),4).data().toString();
		genre = ii.sibling(ii.row(),5).data().toString();
		comment = ii.sibling(ii.row(),6).data().toString();
		track = ii.sibling(ii.row(),7).data().toString();
		
		mttFile f;
		TagLib::Tag *t;
		bool changed = false;
		f.Open(fname);
		t = f.getTag();
		if ( isTagChanged(ii) ) {
		    t->setTitle(Q4StringToTString(title));
		    t->setArtist(Q4StringToTString(artist));
		    t->setAlbum(Q4StringToTString(album));
		    t->setYear(year.toUInt());
		    t->setGenre(Q4StringToTString(genre));
		    t->setComment(Q4StringToTString(comment));
		    t->setTrack(track.toUInt());
		    f.saveTag();
		}
		current++;
		progress.setValue((float) current/count);
	    }
	    children--;
	}
    }

    QApplication::restoreOverrideCursor();
    progress.hide();
    statusBar()->showMessage( tr( "Done" ) );
}

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
        li = renumModel.findItems( l.at(l.count()/num_col+i).data().toString() ); // Take renum model item that matches the filename of the selected treemodel item
        treeModel.setData( l.at( (num_col-1)*l.count()/num_col + i ), QVariant( li[0]->row() + 1 ) );
        qDebug() << renumModel.data( renumModel.index( i, 0 ), Qt::UserRole ).toString();
    }
}

void mttMainWin::setTagChanged( const QModelIndex &index, bool changed )
{
    if (!ignoreChange) {
        treeModel.setData( index.sibling( index.row(), 0 ) , changed, Qt::UserRole ); // Modify changed flag
        if ( changed ) {
    /*        QFont f;
            f.setBold( true );
            f.setUnderline( true );
            treeModel.setData( index.sibling( index.row(), 0 ), f, Qt::FontRole );*/
            //for ( int i=0; i<8; i++ )
                treeModel.setData( index.sibling( index.row(), 0 ), Qt::red, Qt::DecorationRole );
        }
        else {
    /*        QFont f;
            f.setBold( false );
            f.setUnderline( false );
            treeModel.setData( index.sibling( index.row(), 0 ), f, Qt::FontRole );*/
            //for ( int i=0; i<8; i++ )
                treeModel.setData( index.sibling( index.row(), 0 ), QVariant(), Qt::DecorationRole );
        }
    }
}

bool mttMainWin::isTagChanged(const QModelIndex& index)
{
    if ( index.data( Qt::DecorationRole ) != QVariant() )
	return true;
    else
	return false;
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
    actAboutQt = new QAction( tr( "About Qt" ), this );
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
    menuHelp->addAction( actAboutQt );
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

        titleLabel->setText( strList.at( 0 ) );
        artistLabel->setText( strList.at( 1 ) );
        albumLabel->setText( strList.at( 2 ) );
        yearLabel->setText( strList.at( 3 ) );
        genreLabel->setText( strList.at( 4 ) );
        commentLabel->setText( strList.at( 5 ) );
        trackLabel->setText( strList.at( 6 ) );
    }
    else { // Tag to filename
	slotFormatUpdateFnamePreview(treeView->currentIndex());
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
        l << title << artist << album << year << genre << comment << track;
        return l;
}

void mttMainWin::slotFormatEdited()
{
    if ( dockFormat->isVisible() && autoUpd->isChecked() )
        slotFormatUpdatePreview();
}

void mttMainWin::slotFormatUpdateFnamePreview( const QModelIndex &current )
{
    if (current != QModelIndex()) {
	newfnameLabel->setText( getFilenameFromTags(current) );
    }
}

void mttMainWin::slotNotImplemented( void )
{
    QMessageBox::information( this, "Information", "Not implemented yet!" );
}

void mttMainWin::slotItemChanged( QStandardItem *item )
{
    setTagChanged( item->index(), true );
}

void mttMainWin::closeEvent(QCloseEvent *event)
{
    QSettings settings("mediatagtools", "mtt");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void mttMainWin::readSettings()
{
    QSettings settings("mediatagtools", "mtt");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void mttMainWin::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void mttMainWin::slotAbout()
{
    QDialog aboutd;
    Ui::AboutDialog ui;
    ui.setupUi(&aboutd);
    ui.ProgNameLabel->setText(QString(ui.ProgNameLabel->text()).replace(QString("v."), QString("v.") + QString(VERSION)));
    aboutd.exec();
}

void mttMainWin::slotFormatApplyToTags ( void )
{
    QModelIndexList l = treeView->selectionModel()->selectedIndexes();
    QStringList strList, titles, artists, albums, years, genres, comments, tracks;
    QString filename;
    int count = l.count() / num_col;
    
    
    for (int i=0; i<count; i++) {
	filename = ( l.at(i).sibling( l.at(i).row(), 0 ) ).data( Qt::DisplayRole ).toString();
        // Remove the extension from the filename
	int j;
        for ( j = ( filename.length() - 1 ); j >= 0; j-- )
            if ( filename[j] == '.' )
                break;
        if ( j )
            filename = filename.left( j );
	
	strList = getTagsFromFilename( filename );
	titles << strList.at(0);
	artists << strList.at(1);
	albums << strList.at(2);
	years << strList.at(3);
	genres << strList.at(4);
	comments << strList.at(5);
	tracks << strList.at(6);
	setTagChanged( l.at(i) );
    }
    for (int i=0; i<count; i++) {
	treeModel.setData( l.at(count+i), titles.at(i) );
	treeModel.setData( l.at(2*count+i), artists.at(i) );
	treeModel.setData( l.at(3*count+i), albums.at(i) );
	treeModel.setData( l.at(4*count+i), years.at(i) );
	treeModel.setData( l.at(5*count+i), genres.at(i) );
	treeModel.setData( l.at(6*count+i), comments.at(i) );
	treeModel.setData( l.at(7*count+i), tracks.at(i) );
    }
}

void mttMainWin::slotFormatApplyToFilenames ( void )
{
    QModelIndexList l;
    QString curfname, curfnamepath, newfname;
    QStringList curl, newl;
    int count;
    float prog, step;
    
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    statusBar()->showMessage( tr("Renaming...") );
    progress.setRange(0,100);
    l = treeView->selectionModel()->selectedIndexes();
    count = l.count()/num_col;
    prog = 0;
    step = (float) 100 / count;
    for (int i=0; i<count; i++) {
	curfname = l.at(i).model()->data( l.at(i).sibling( l.at(i).row(), 0 ) ).toString();
	curfnamepath = l.at(i).model()->data( l.at(i).parent() ).toString();
	newfname = getFilenameFromTags(l.at(i));
	curl << curfnamepath + "/" + curfname;
	newl << curfnamepath + "/" + newfname;
	//QMessageBox::warning( this, QString("Warning"), QString("Renaming ") + curfnamepath + "/" + curfname + " to " + curfnamepath + "/" + newfname );
    }
    // If there are files that will have the same name after applying the filter...
    if (newl.removeDuplicates()) {
	QMessageBox::critical( this, QString( tr("Error!") ), QString( tr("Two or more files will have the same filename. Please check the filter and tags before trying again.") ) );
    }
    else {
	int i=0;
	while (!abort && i<count) {
	    bool abort = false;
	    progress.setValue(prog);
	    if (QFile::rename( curl.at(i), newl.at(i) ) != true) { // Error while renaming
		if (QFile::exists(newl.at(i))) {
		    if (QMessageBox::critical(this, QString("Error!"), QString("File ")+newl.at(i)+" already exists!", QMessageBox::Ok, QMessageBox::Abort) == QMessageBox::Abort)
			abort = true;
		}
		else
		    if (QMessageBox::critical(this, QString("Error!"), QString("Renaming ")+curl.at(i)+" to "+newl.at(i)+" failed!", QMessageBox::Ok, QMessageBox::Abort) == QMessageBox::Abort)
			abort = true;
	    }
	    prog += step;
	    i++;
	}
    }
    progress.setValue(100);
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage( tr( "Done" ) );
}

QString mttMainWin::getFilenameFromTags(const QModelIndex& current)
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
    // Add file extension
    newfname += curfname.right( curfname.length() - curfname.lastIndexOf( "." ) );
    
    return newfname;
}

void mttMainWin::slotEditAdvTagTableItem(QTableWidgetItem* item)
{
    if (item->column() == 0)
	item->tableWidget()->editItem(item);
}
