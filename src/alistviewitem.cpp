//
// C++ Implementation: alistviewitem
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qfile.h>

#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <textidentificationframe.h>
#include <tstring.h>

#include "alistviewitem.h"

AListViewItem::AListViewItem( QListView * parent )
 : QListViewItem( parent )
{
    fileref = NULL;
    ismpeg = false;
    isogg = false;
    isflac = false;
    tagChange = false;
}

AListViewItem::AListViewItem ( QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
 : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
    fileref = NULL;
    ismpeg = false;
    isogg = false;
    isflac = false;
    tagChange = false;
}


AListViewItem::~AListViewItem()
{
    if ( fileref )
        delete fileref;
}

void AListViewItem::FileRef( QString filename )
{
    fname = filename;
    fileref = new TagLib::FileRef( QFile::encodeName( filename ) );
    if ( filename.endsWith( ".mp3", false ) ) {
        ismpeg = true;
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        if ( f ) {
            if ( !f->ID3v2Tag() && f->ID3v1Tag() ) {
                // Copy id3v1 tag to id3v2 tag
                TagLib::ID3v1::Tag *v1tag = f->ID3v1Tag();
                TagLib::ID3v2::Tag *v2tag = f->ID3v2Tag( true );

                v2tag->setAlbum( v1tag->album() );
                v2tag->setArtist( v1tag->artist() );
                v2tag->setComment( v1tag->comment() );
                v2tag->setGenre( v1tag->genre() );
                v2tag->setTitle( v1tag->title() );
                v2tag->setTrack( v1tag->track() );
                v2tag->setYear( v1tag->year() );
                setTagChanged( true );
            }
            else if ( f->ID3v2Tag() && f->ID3v1Tag() ) {
                // Fill gaps of ID3v2Tag from ID3v1Tag
                TagLib::ID3v1::Tag *v1tag = f->ID3v1Tag();
                TagLib::ID3v2::Tag *v2tag = f->ID3v2Tag( true );

                if ( v2tag->album().isEmpty() ) {
                    v2tag->setAlbum( v1tag->album() );
                    setTagChanged( true );
                }
                if ( v2tag->artist().isEmpty() ) {
                    v2tag->setArtist( v1tag->artist() );
                    setTagChanged( true );
                }
                if ( v2tag->comment().isEmpty() ) {
                    v2tag->setComment( v1tag->comment() );
                    setTagChanged( true );
                }
                if ( v2tag->genre().isEmpty() ) {
                    v2tag->setGenre( v1tag->genre() );
                    setTagChanged( true );
                }
                if ( v2tag->title().isEmpty() ) {
                    v2tag->setTitle( v1tag->title() );
                    setTagChanged( true );
                }
                if ( v2tag->track() == 0 ) {
                    v2tag->setTrack( v1tag->track() );
                    setTagChanged( true );
                }
                if ( v2tag->year() == 0 ) {
                    v2tag->setYear( v1tag->year() );
                    setTagChanged( true );
                }
            }

            // Remove id3v1 tag. Help put that hack into eternal rest :-)
            //f->strip( TagLib::MPEG::File::ID3v1 );
        }
    }
    else if ( filename.endsWith( ".ogg", false ) ) {
        isogg = true;
    }
    else if ( filename.endsWith( ".flac", false ) ) {
        isflac = true;
    }
}

TagLib::Tag *AListViewItem::getTag( void )
{
    if ( fileref )
        if ( !ismpeg )
            return fileref->tag();
        else {
            qDebug( "ismpeg" );
            TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
            return dynamic_cast<TagLib::Tag *>( f->ID3v2Tag( true ) );
        }
    else {
        qDebug( "fileref = NULL" );
        return NULL;
    }
}

void AListViewItem::saveTag( void )
{
    fileref->save();
}

void AListViewItem::removeTag( void )
{
    if ( ismpeg ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        f->strip();
    }
    else {
        TagLib::Tag *tag = fileref->tag();

        tag->setTitle( TagLib::String::null );
        tag->setArtist( TagLib::String::null );
        tag->setAlbum( TagLib::String::null );
        tag->setComment( TagLib::String::null );
        tag->setGenre( TagLib::String::null );
        tag->setYear( 0 );
        tag->setTrack( 0 );
        fileref->save();
    }
}

QString AListViewItem::getFName( void )
{
    return fname;
}

bool AListViewItem::isMpeg( void )
{
    return ismpeg;
}

bool AListViewItem::isOgg( void )
{
    return isogg;
}

bool AListViewItem::isFLAC( void )
{
    return isflac;
}

TagLib::ID3v2::Tag *AListViewItem::getID3Tag( bool create )
{
    if ( ismpeg ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        return f->ID3v2Tag( create );
    }
    else
        return NULL;
}

void AListViewItem::checkEncodings( void )
{
    if ( isMpeg() ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());

        TagLib::ID3v2::FrameList l = f->ID3v2Tag()->frameListMap()["TALB"];
        if ( !l.isEmpty() ) {
            TagLib::ID3v2::TextIdentificationFrame *tf = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(l.front());
            if ( tf && tf->textEncoding() != TagLib::String::UTF8 )
                tf->setTextEncoding( TagLib::String::UTF8 );
        }

        l = f->ID3v2Tag()->frameListMap()["TIT2"];
        if ( !l.isEmpty() ) {
            TagLib::ID3v2::TextIdentificationFrame *tf = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(l.front());
            if ( tf && tf->textEncoding() != TagLib::String::UTF8 )
                tf->setTextEncoding( TagLib::String::UTF8 );
        }

        l = f->ID3v2Tag()->frameListMap()["TPE1"];
        if ( !l.isEmpty() ) {
            TagLib::ID3v2::TextIdentificationFrame *tf = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(l.front());
            if ( tf && tf->textEncoding() != TagLib::String::UTF8 )
                tf->setTextEncoding( TagLib::String::UTF8 );
        }

        l = f->ID3v2Tag()->frameListMap()["TCON"];
        if ( !l.isEmpty() ) {
            TagLib::ID3v2::TextIdentificationFrame *tf = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(l.front());
            if ( tf && tf->textEncoding() != TagLib::String::UTF8 )
                tf->setTextEncoding( TagLib::String::UTF8 );
        }

        l = f->ID3v2Tag()->frameListMap()["COMM"];
        if ( !l.isEmpty() ) {
            TagLib::ID3v2::TextIdentificationFrame *tf = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame *>(l.front());
            if ( tf && tf->textEncoding() != TagLib::String::UTF8 )
                tf->setTextEncoding( TagLib::String::UTF8 );
        }
    }
}

void AListViewItem::setTagChanged( bool ch )
{
    tagChange = ch;
}

bool AListViewItem::tagChanged( void )
{
    return tagChange;
}

void AListViewItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
    // A rip of the default paintCell function
    // All I ever wanted was a way to have color text in a QListView...
    // Change width() if you change this.

    if ( !p )
        return;

    QListView *lv = listView();
    if ( !lv )
        return;

//     const BackgroundMode bgmode = lv->viewport()->backgroundMode();
//     const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( bgmode );
//     if ( cg.brush( crole ) != lv->colorGroup().brush( crole ) )
//         p->fillRect( 0, 0, width, height(), cg.brush( crole ) );
//     else
//         lv->paintEmptyArea( p, QRect( 0, 0, width, height() ) );

//     bool parentControl = FALSE;
//     if ( parent() && parent()->rtti() == 1  &&
//          ((QCheckListItem*) parent())->type() == RadioButtonController )
//         parentControl = TRUE;

    QFontMetrics fm( lv->fontMetrics() );
    int marg = lv->itemMargin();
    int r = marg;

    // Draw text ----------------------------------------------------
    p->translate( r, 0 );
    if ( tagChanged() ) {
        QColorGroup cg2 = cg;
        cg2.setColor( QColorGroup::Text, Qt::red );
        cg2.setColor( QColorGroup::HighlightedText, Qt::red );
        QListViewItem::paintCell( p, cg2, column, width - r, align );
    }
    else
        QListViewItem::paintCell( p, cg, column, width - r, align );
}
