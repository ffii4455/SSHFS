#include "mainwindow.h"
#include "ui_mainwindow.h"
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

void MainWindow::on_startConnect_clicked()
{
//    fs.reset(new fsThread());
//    fs->start();

    ss.reset(new sshThread());
    ss->setSshPara(ui->hostAddr->text(),
                   ui->port->value(),
                   ui->username->text(),
                   ui->password->text(),
                   ui->rootPath->text());
    ss->start();
}
