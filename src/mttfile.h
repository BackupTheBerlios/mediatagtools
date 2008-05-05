//
// C++ Interface: alistviewitem
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

#include <QtCore/QStringList>
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

    void Open( QString filename );
    void removeTag( void );
    TagLib::Tag *getTag( bool create = false );
    void saveTag( void );
    QString getFName( void );
    void setFName( QString );
    //TagLib::ID3v2::Tag *getID3Tag( bool create = false );
    void checkEncodings( void );
    void setTagChanged( bool ); // TODO: Implementation of setTagChanged

    void setMp3ExtraFrames( QStringList );
    QStringList getMp3ExtraFrames( void );

    //Checks
    bool isMpeg( void );
    bool isOgg( void );
    bool isFLAC( void );
    bool tagChanged( void );

private:
    TagLib::FileRef *fileref;
    bool ismpeg, isogg, isflac;
    QString fname;
    bool tagChange;
    TagLib::Tag *tag;
    QStringList mp3eframes;
};

#endif
