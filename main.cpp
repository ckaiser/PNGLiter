#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("K");
    a.setApplicationName("PNGLiter");
    a.setApplicationVersion("0.1");

    MainWindow w;

    if (!w.isEnabled())
    { // OptiPNG not found.
      return 0;
    }

    w.show();

    return a.exec();
}
