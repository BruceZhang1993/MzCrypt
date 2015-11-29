#include <QApplication>

#include "maingui.h"

int
main(int argc, char **argv)
{

  QApplication app(argc, argv);
  app.setApplicationName("MzCrypt-Gui");
  app.setApplicationVersion("1.0.3");
  app.setOrganizationName("iSilver");
  app.setOrganizationDomain("iSilver");

  Main_GUI gui;
  gui.show();

  return app.exec();

}

// end of main.cpp
