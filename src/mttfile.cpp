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

#include <QtCore/QFile>

#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <xiphcomment.h>
#include <textidentificationframe.h>
#include <tstring.h>

#include "mttfile.h"

#include "mp3extraframes.h"
#include "tools.h"

mttFile::mttFile()
{
    fileref = NULL;
    ismpeg = false;
    isogg = false;
    isflac = false;
    tagChange = false;
    tag = NULL;
}

mttFile::~mttFile()
{
    if ( fileref )
        delete fileref;
    if ( tag )
        delete tag;
}

void mttFile::Open( QString filename )
{
    fname = filename;

	fileref = new TagLib::FileRef( QFile::encodeName( filename ).constData() );
    if ( filename.endsWith( QString( ".mp3" ), Qt::CaseInsensitive ) ) {
        ismpeg = true;
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        if ( f ) {
            if ( f->ID3v1Tag() ) {
                if ( !(f->ID3v2Tag()) ) {
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
                    TagLib::ID3v2::Tag *v2tag = f->ID3v2Tag();
    
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
            }
        }
    }
    else if ( filename.endsWith( QString( ".ogg" ), Qt::CaseInsensitive ) ) {
        isogg = true;
    }
    else if ( filename.endsWith( QString( ".flac" ), Qt::CaseInsensitive ) ) {
        isflac = true;
    }

    delete fileref;
    fileref = NULL;
}

TagLib::Tag *mttFile::getTag( bool create )
{
    if ( tag == NULL ) {
		fileref = new TagLib::FileRef( QFile::encodeName( fname ).constData() );

        if ( fileref ) {
            if ( ismpeg ) {
                TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
                tag = new TagLib::ID3v2::Tag();
                if ( f->ID3v2Tag( create ) != NULL ) {
                    TagLib::Tag::duplicate( dynamic_cast<TagLib::Tag *>( f->ID3v2Tag( create ) ), tag, true );

                    // Read extra mp3 tags
                    int i;
                    for ( i = 0; i < EF_NUM; i++ ) {
                        TagLib::ID3v2::FrameList l = f->ID3v2Tag()->frameListMap()[extraFrames[i][0]];

                        if ( !l.isEmpty() ) {
                            mp3eframes += extraFrames[i][0];
                            mp3eframes += TStringToQString( l.front()->toString() );
                        }
                    }
                }


                delete fileref;
                fileref = NULL;
                return tag;
            }
            else { // If the file is ogg or flac
                tag = new TagLib::Ogg::XiphComment();
                if ( fileref->tag() ) // If a tag already exists
                    TagLib::Tag::duplicate( fileref->tag(), tag, true );
                delete fileref;
                fileref = NULL;
                return tag;
            }
        }
        else {
            //qDebug( "fileref = NULL" );
            return NULL;
        }

        delete fileref;
        fileref = NULL;
    }
    else
        return tag;
}

void mttFile::saveTag( void )
{
	fileref = new TagLib::FileRef( QFile::encodeName( fname ).constData() );

    if ( ismpeg ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());

        // Remove id3v1 tag. Help put that hack into eternal rest :-)
        f->strip( TagLib::MPEG::File::ID3v1, true );
        //qDebug("ID3v1 tag stripped!");

        TagLib::Tag::duplicate( tag, dynamic_cast<TagLib::Tag *>( f->ID3v2Tag( true ) ), true );

        // Save extra mp3 tags
        TagLib::ID3v2::TextIdentificationFrame *myframe;
        TagLib::ID3v2::Tag *mytag;

        mytag = f->ID3v2Tag( false );
        for ( QStringList::Iterator it = mp3eframes.begin(); it != mp3eframes.end(); ++it ) {
            myframe = new TagLib::ID3v2::TextIdentificationFrame( (*it).toAscii().constData(), TagLib::String::UTF8 );
            mytag->removeFrames( (*it).toAscii().constData() );
            ++it;
            myframe->setText( Q4StringToTString( (*it) ) );
            mytag->addFrame( myframe );
        }

        // TODO: Handle save errors
        //if ( !f->save( TagLib::MPEG::File::ID3v2, true ) );
            //qDebug( "Error" );
    }
    else {
        TagLib::Tag::duplicate( tag, fileref->tag(), true );
        fileref->save();
    }

    delete fileref;
    fileref = NULL;
}

void mttFile::removeTag( void )
{
	fileref = new TagLib::FileRef( QFile::encodeName( fname ).constData() );

    if ( ismpeg ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
        f->strip();
    }
    else {
        TagLib::Tag *intag = fileref->tag();

        intag->setTitle( TagLib::String::null );
        intag->setArtist( TagLib::String::null );
        intag->setAlbum( TagLib::String::null );
        intag->setComment( TagLib::String::null );
        intag->setGenre( TagLib::String::null );
        intag->setYear( 0 );
        intag->setTrack( 0 );
        fileref->save();
    }

    delete fileref;
    fileref = NULL;
}

QString mttFile::getFName( void )
{
    return fname;
}

void mttFile::setFName( QString newname )
{
    fname = newname;
}

bool mttFile::isMpeg( void )
{
    return ismpeg;
}

bool mttFile::isOgg( void )
{
    return isogg;
}

bool mttFile::isFLAC( void )
{
    return isflac;
}

// TagLib::ID3v2::Tag *mttFile::getID3Tag( bool create )
// {
//     fileref = new TagLib::FileRef( QFile::encodeName( fname ) );
// 
//     if ( ismpeg ) {
//         TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());
//         return f->ID3v2Tag( create );
//     }
//     else
//         return NULL;
// 
//     delete fileref;
//     fileref = NULL;
// }

void mttFile::checkEncodings( void )
{
	fileref = new TagLib::FileRef( QFile::encodeName( fname ).constData() );

    if ( isMpeg() ) {
        TagLib::MPEG::File *f = dynamic_cast<TagLib::MPEG::File *>(fileref->file());

        if ( f->ID3v2Tag() ) {
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

    delete fileref;
    fileref = NULL;
}

void mttFile::setTagChanged( bool ch )
{
    tagChange = ch;
/*    QStandardItemModel *sim = model();

    if ( ch ) {
        for ( int i = 0; i < COLUMNS; i++ ) {
            sim->item( index().row(), index().column() + i )->setForeground( QBrush( Qt::red ) );
        }
    }
    else {
        for ( int i = 0; i < COLUMNS; i++ ) {
            sim->item( index().row(), index().column() + i )->setForeground( QBrush( Qt::NoBrush ) );
        }
    }*/
}

bool mttFile::tagChanged( void )
{
    return tagChange;
}

void mttFile::setMp3ExtraFrames( QStringList ef )
{
    mp3eframes = ef;
}

QStringList mttFile::getMp3ExtraFrames( void )
{
    return mp3eframes;
}
