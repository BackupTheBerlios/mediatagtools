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
}

void mttCFDialog::slotTrackChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<track>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<track>" ) ) );
    slotUpdateFormat();
}


void mttCFDialog::slotCommentChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<comment>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<comment>" ) ) );
    slotUpdateFormat();
}


void mttCFDialog::slotYearChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<year>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<year>" ) ) );
    slotUpdateFormat();
}


void mttCFDialog::slotTitleChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<title>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<title>" ) ) );
    slotUpdateFormat();
}


void mttCFDialog::slotAlbumChkB( int state )
{
    if ( state == QButton::On )
        listBox->insertItem( "<album>" );
    else if ( state == QButton::Off )
        listBox->removeItem( listBox->index( listBox->findItem( "<album>" ) ) );
    slotUpdateFormat();
}

void mttCFDialog::setFormat( QString formatstr )
{
    CFormatLE->setText( formatstr );
    if ( formatstr.find( "<artist>" ) != -1 )
        ArtistChkB->setChecked( true );
    if ( formatstr.find( "<album>" ) != -1 )
        AlbumChkB->setChecked( true );
    if ( formatstr.find( "<title>" ) != -1 )
        TitleChkB->setChecked( true );
    if ( formatstr.find( "<year>" ) != -1 )
        YearChkB->setChecked( true );
    if ( formatstr.find( "<comment>" ) != -1 )
        CommentChkB->setChecked( true );
    if ( formatstr.find( "<track>" ) != -1 )
        TrackChkB->setChecked( true );
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

void mttCFDialog::slotLZChkB( int check )
{
    LZ = check;
}

bool mttCFDialog::isLZOn( void )
{
    if ( LZ == QButton::On )
        return true;
    else
        return false;
}

QString mttCFDialog::getLZ1( void )
{
    return LZ1LE->text();
}

QString mttCFDialog::getLZ2( void )
{
    return LZ2LE->text();
}
