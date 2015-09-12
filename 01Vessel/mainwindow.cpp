#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    setGeometry(300,100,800,600);
    setWindowTitle("PhD Bullet - Soft tissue Deformation");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    static bool b_fullScreen = false;
    if(e->key() == Qt::Key_Escape)
        exit(0);
    else if(e->key() == Qt::Key_F){
        if(!b_fullScreen){
            showFullScreen();
            b_fullScreen = true;
        }
        else{
            showNormal();
            b_fullScreen = false;
        }
    }
}
