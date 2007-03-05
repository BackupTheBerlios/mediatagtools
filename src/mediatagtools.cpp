#include <config.h>

#include <iostream>
#include <qapplication.h>
#include <qtextcodec.h>
#include <qimage.h>

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

  if ( argc > 1 ) {
    if ( QFile::exists( argv[1] ) ) {
        m.openDir( QString( argv[1] ) );
    }
  }

  a.exec();

  return EXIT_SUCCESS;
}
