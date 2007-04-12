#include <config.h>

#include <iostream>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qimage.h>
#include <qdir.h>

#include "mttmainwin.h"

int main(int argc, char *argv[])
{
  QApplication a( argc, argv );

  // translation file for Qt
  QTranslator qt( 0 );
  qt.load( QString( "qt_" ) + QTextCodec::locale() );
  a.installTranslator( &qt );

  // translation file for application strings
  QTranslator myapp( 0 );
  if ( !myapp.load( QString( "mtt_" ) + QTextCodec::locale(), "/usr/local/share/mediatagtools" ) )
    myapp.load( QString( "mtt_" ) + QTextCodec::locale(), "/usr/share/mediatagtools" );
  a.installTranslator( &myapp );

  mttMainWin m;
  a.setMainWidget( &m );

  // Window icon (get rid of that pink/purple X!)
  QPixmap icon( QImage( QString( QString( PREFIX ) + "/share/mediatagtools/icons/mediatagtools.png" ) ) );
  m.setIcon( icon );

  m.show();

//   qDebug( QString( "argv[1]=\"" ) + argv[1] + "\"\n" );
  if ( argc > 1 ) {
    QDir td;
    td.setPath( QString::fromUtf8( argv[1] ) );
    if ( td.exists() ) {
//         qDebug( "Exists!" );
        m.addDir( QString::fromUtf8( argv[1] ) );
    }
  }

  a.exec();

  return EXIT_SUCCESS;
}
