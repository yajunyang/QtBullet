#include "mainwindow.h"
#include <QApplication>
#include "glwidget.h"
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    GLWidget g;
    g.show();

    qDebug("here");

    return a.exec();
}
