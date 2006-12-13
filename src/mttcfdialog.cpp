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

#include <qlistbox.h>
#include <qbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qpushbutton.h>
#include <qgroupbox.h>

#include "mttcfdialog.h"

mttCFDialog::mttCFDialog(QWidget* parent, const char* name, bool modal, WFlags fl)
: CFormat(parent,name, modal,fl)
{
}

mttCFDialog::~mttCFDialog()
{
}

/*$SPECIALIZATION$*/
void mttCFDialog::reject()
{
  QDialog::reject();
}

void mttCFDialog::accept()
{
  QDialog::accept();
}

void mttCFDialog::slotArtistChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<artist>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<artist>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}

void mttCFDialog::slotTrackChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<track>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<track>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}


void mttCFDialog::slotCommentChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<comment>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<comment>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}


void mttCFDialog::slotYearChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<year>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<year>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}


void mttCFDialog::slotTitleChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<title>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<title>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}


void mttCFDialog::slotAlbumChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<album>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<album>" ) ) );
    slotUpdateFormat();

    if ( listBox->count() ) {
        MoveUpButton->setEnabled( true );
        MoveDownButton->setEnabled( true );
        RemoveAllButton->setEnabled( true );
    }
    else {
        MoveUpButton->setEnabled( false );
        MoveDownButton->setEnabled( false );
        RemoveAllButton->setEnabled( false );
    }
}

void mttCFDialog::setFormat( QString formatstr )
{
    CFormatLE->setText( formatstr );

    int idx = 0, offset = 0;
    QRegExp rx("<(artist|album|title|track|comment)>");
    QString str;

    while ((idx = formatstr.find(rx, idx + offset)) != -1) {
      str = rx.cap(1);
      offset = str.length() + 2;
      if (str == "artist") ArtistChkB->setChecked(true);
      else if (str == "track") TrackChkB->setChecked(true);
      else if (str == "title") TitleChkB->setChecked(true);
      else if (str == "year") YearChkB->setChecked(true);
      else if (str == "album") AlbumChkB->setChecked(true);
      else if (str == "comment") CommentChkB->setChecked(true);
      else qDebug( "Invalid format field: " + QString::number( idx ) +  " + " + QString::number( offset ) + ": " + str );
    }
}

QString mttCFDialog::getFormat( void )
{
    return CFormatLE->text();
}

void mttCFDialog::setSeparators( QStringList strlist )
{
    QStringList::Iterator it = strlist.begin();

    Separator1->setText( *it );
    ++it;
    Separator2->setText( *it );
    ++it;
    Separator3->setText( *it );
    ++it;
    Separator4->setText( *it );
    ++it;
    Separator5->setText( *it );
    ++it;
}

QStringList mttCFDialog::getSeparators( void )
{
    QStringList strlist;

    strlist << Separator1->text();
    strlist << Separator2->text();
    strlist << Separator3->text();
    strlist << Separator4->text();
    strlist << Separator5->text();
    return strlist;
}

void mttCFDialog::slotUpdateFormat()
{
    QString formatstr = "";
    unsigned int i;

    for ( i = 0; i < listBox->count(); i++ ) {
        formatstr += listBox->text( i );
        if ( ( listBox->count() - i ) > 1 )
            switch ( i ) {
                case 0:
                    formatstr += Separator1->text();
                    break;
                case 1:
                    formatstr += Separator2->text();
                    break;
                case 2:
                    formatstr += Separator3->text();
                    break;
                case 3:
                    formatstr += Separator4->text();
                    break;
                case 4:
                    formatstr += Separator5->text();
                    break;
        }
    }

    CFormatLE->setText( formatstr );
}

void mttCFDialog::slotRemoveAllBtn()
{
    ArtistChkB->setChecked( false );
    AlbumChkB->setChecked( false );
    TitleChkB->setChecked( false );
    YearChkB->setChecked( false );
    TrackChkB->setChecked( false );
    CommentChkB->setChecked( false );
}

void mttCFDialog::slotMoveDownBtn()
{
    QString txt;

    // If something is selected and it isn't the last item...
    if ( ( listBox->currentItem() != -1 ) && ( ( ( unsigned int ) listBox->currentItem() ) != ( listBox->count() - 1 ) ) ) {
        txt = listBox->text( listBox->currentItem() + 1 );
        listBox->changeItem( listBox->currentText(), listBox->currentItem() + 1 );
        listBox->changeItem( txt , listBox->currentItem() - 1 );
        listBox->setCurrentItem( listBox->currentItem() + 1 );
        slotUpdateFormat();
    }
}


void mttCFDialog::slotMoveUpBtn()
{
    QString txt;

    // If something is selected and it isn't the first item...
    if ( ( listBox->currentItem() != -1 ) && ( listBox->currentItem() != 0 ) ) {
        txt = listBox->text( listBox->currentItem() - 1 );
        listBox->changeItem( listBox->currentText(), listBox->currentItem() - 1 );
        listBox->changeItem( txt , listBox->currentItem() + 1 );
        listBox->setCurrentItem( listBox->currentItem() - 1 );
        slotUpdateFormat();
    }
}

