#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <qapplication.h>

#include "mttmainwin.h"

int main(int argc, char *argv[])
{
  QApplication a( argc, argv );
  mttMainWin m;
  a.setMainWidget( &m );
  m.show();
  a.exec();

  return EXIT_SUCCESS;
}
