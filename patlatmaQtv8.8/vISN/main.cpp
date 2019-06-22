#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("./");
    QApplication a(argc, argv);
    MainWindow w;
    //w.setWindowFlags( Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
    w.setWindowFlags( Qt::Window | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
  //  QRect r1(0,40,800,600);
  //  w.setGeometry(r1);

  //  w.show();
    w.showFullScreen();

   // w.showNormal();
    return a.exec();
}
