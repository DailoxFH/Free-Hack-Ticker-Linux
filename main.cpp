#include "./inc/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FH-Ticker");
    MainWindow w;
    w.show();
    return a.exec();
}