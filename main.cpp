#include "./inc/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FH-Ticker");
    MainWindow w;
    if(w.loadSettings.value("startHidden").toInt() == 1) {
        w.hide();
    } else {
        w.show();
    }
    return a.exec();
}
