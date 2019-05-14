#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //Здесь мы просто запускаем окно с игрой.

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
