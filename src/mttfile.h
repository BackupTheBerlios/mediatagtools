//
// C++ Interface: mtt
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MTTFILE_H
#define MTTFILE_H

#include <QStringList>
#include <fileref.h>

#define COLUMNS 8


/**
@author Theofilos Intzoglou
*/
class mttFile
{
public:
    mttFile();

    ~mttFile();

    bool Open( QString filename ); // Returns true if tag has changed
    void removeTag( void );
    TagLib::Tag *getTag( bool create = false );
    void saveTag( void );
    QString getFName( void );
    void setFName( QString );
    //TagLib::ID3v2::Tag *getID3Tag( bool create = false );
    void checkEncodings( void );
	TagLib::AudioProperties *getAudioProperties( void );

    void setMp3ExtraFrames( QStringList );
    QStringList getMp3ExtraFrames( void );

    //Checks
    bool isMpeg( void );
    bool isOgg( void );
    bool isFLAC( void );

private:
    TagLib::FileRef *fileref;
    bool ismpeg, isogg, isflac;
    QString fname;
    TagLib::Tag *tag;
    QStringList mp3eframes;
};

#endif
