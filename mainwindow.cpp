#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <WinBase.h>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    fs.reset(new fsThread(this));
    fs->start();
}
