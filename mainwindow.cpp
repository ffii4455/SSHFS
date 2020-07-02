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
    fs.reset(new FsThread());
    fs->start();

    ss.reset(new SshThread());
    ss->setSshPara(ui->hostAddr->text(),
                   ui->port->value(),
                   ui->username->text(),
                   ui->password->text(),
                   ui->rootPath->text());
    ss->start();
    connect(&fsys, &FileSystem::openDir, ss.get(), &SshThread::openDir);
}
