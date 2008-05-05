//#include <config.h>

//#include <iostream>
#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtGui/QImage>
#include <QtCore/QDir>
//Added by qt3to4:
#include <QtGui/QPixmap>
#include <QtCore/QTranslator>

#include "mttmainwin.h"

int main(int argc, char *argv[])
{
  QApplication a( argc, argv );

  // translation file for Qt
  //QTranslator qt;
  QLocale s( QLocale::system() );
  //qDebug( QString( s.name() ).toStdString().data() );
  //qt.load( QString( "qt_" ) + s.name() );
  //a.installTranslator( &qt );

  // translation file for application strings
  QTranslator myapp( 0 );
  if ( !myapp.load( QString( "mtt_" ) + s.name(), "/usr/local/share/mediatagtools" ) )
    myapp.load( QString( "mtt_" ) + s.name(), "/usr/share/mediatagtools" );
  a.installTranslator( &myapp );

  mttMainWin m;

  // TODO: Window icon (get rid of that pink/purple X!)
  //QPixmap icon( QImage( QString( QString( PREFIX ) + "/share/mediatagtools/icons/mediatagtools.png" ) ) );
  //QPixmap icon( QString( QString( PREFIX ) + "/share/mediatagtools/icons/mediatagtools.png" ) );
  //m.setIcon( icon );

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
