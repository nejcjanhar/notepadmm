#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/qdarkstyle/style.qss");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            a.setStyleSheet(file.readAll());
            file.close();
        }


    MainWindow w;

    w.show();

    return a.exec();
}
