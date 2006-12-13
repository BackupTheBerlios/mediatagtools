//
// C++ Interface: 
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ATTCFDIALOG_H
#define ATTCFDIALOG_H

#include <qobject.h>
#include "form2.h"

class mttCFDialog : public CFormat
{
  Q_OBJECT

public:
  mttCFDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~mttCFDialog();
  /*$PUBLIC_FUNCTIONS$*/
    void setFormat( QString );
    QString getFormat( void );
    void setSeparators( QStringList );
    QStringList getSeparators( void ); // Returns the seperators used in custom format

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
    int LZ;

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void reject();
    virtual void accept();

    virtual void slotArtistChkB( int );
    virtual void slotTrackChkB( int );
    virtual void slotCommentChkB( int );
    virtual void slotYearChkB( int );
    virtual void slotTitleChkB( int );
    virtual void slotAlbumChkB( int );
    virtual void slotUpdateFormat();
    virtual void slotRemoveAllBtn();
    virtual void slotMoveDownBtn();
    virtual void slotMoveUpBtn();
};

#endif

