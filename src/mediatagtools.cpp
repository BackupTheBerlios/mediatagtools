#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <qapplication.h>
#include <qtextcodec.h>

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
  myapp.load( QString( "mtt_" ) + QTextCodec::locale() );
  a.installTranslator( &myapp );

  mttMainWin m;
  a.setMainWidget( &m );
  m.show();
  a.exec();

  return EXIT_SUCCESS;
}
