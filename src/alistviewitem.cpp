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

#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>

#include "alistviewitem.h"

AListViewItem::AListViewItem( QListView * parent )
 : QListViewItem( parent )
{
    fileref = NULL;
}

AListViewItem::AListViewItem ( QListView * parent, QString label1, QString label2, QString label3, QString label4, QString label5, QString label6, QString label7, QString label8 )
 : QListViewItem( parent, label1, label2, label3, label4, label5, label6, label7, label8 )
{
    fileref = NULL;
}


AListViewItem::~AListViewItem()
{
}

void AListViewItem::FileRef( QString filename )
{
    fname = filename;
    fileref = new TagLib::FileRef( filename );
    if ( filename.endsWith( ".mp3", false ) ) {
        ismp3 = true;
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
            }

            // Remove id3v1 tag. Help put that hack into eternal rest :-)
            f->strip( TagLib::MPEG::File::ID3v1 );
        }
    }
    else
        ismp3 = false;
}

TagLib::Tag *AListViewItem::getTag( void )
{
    if ( fileref )
        if ( !ismp3 )
            return fileref->tag();
        else {
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
    if ( ismp3 ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        f->strip();
    }
}

QString AListViewItem::getFName( void )
{
    return fname;
}
