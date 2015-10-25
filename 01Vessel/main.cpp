#include "mainwindow.h"
#include <QApplication>
#include "glwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setCentralWidget(new GLWidget);
    w.show();
    return a.exec();
}
