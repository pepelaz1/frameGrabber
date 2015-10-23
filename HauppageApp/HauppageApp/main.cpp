#include <QApplication>
#include <QDesktopWidget>
#include <QDate>
#include "mainwindow.h"
#include "serialportreader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(800,630);




    QRect screenGeometry = QApplication::desktop()->availableGeometry();

    int x = (screenGeometry.width()-w.width()) / 2;
    int y = (screenGeometry.height()-w.height()) / 2;
    w.move(x, y);

    w.setWindowTitle("HauppageApp");
    w.show();
    w.listen();

    return a.exec();
}
