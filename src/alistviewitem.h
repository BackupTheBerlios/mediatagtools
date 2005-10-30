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
#ifndef ALISTVIEWITEM_H
#define ALISTVIEWITEM_H

#include <qlistview.h>
#include <fileref.h>

/**
@author Theofilos Intzoglou
*/
class AListViewItem : public QListViewItem
{
public:
    AListViewItem( QListView * parent );
    AListViewItem ( QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

    ~AListViewItem();

    void FileRef( QString filename );
    void removeTag( void );
    TagLib::Tag *getTag( void );
    void saveTag( void );
    QString getFName( void );
    bool isMpeg( void );
    bool isOgg( void );
    bool isFLAC( void );
    TagLib::ID3v2::Tag *getID3Tag( bool create = false );

private:
    TagLib::FileRef *fileref;
    bool ismpeg, isogg, isflac;
    QString fname;
};

#endif
