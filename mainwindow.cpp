#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startConnect_clicked()
{
    HostInfo info;

    info.hostAddr = ui->hostAddr->text() + ":" + QString::number(ui->port->value());
    info.password = ui->password->text();
    info.userName = ui->username->text();
    info.rootPath = ui->rootPath->text();

    dokanyThread.reset(new DokanyThread(info));
    dokanyThread->start();
    writeSettings();
}

void MainWindow::on_saveButton_clicked()
{
    writeSettings();
}

void MainWindow::readSettings()
{
    QFile file("settings.json");

    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject json = doc.object();
    QJsonArray array = json.value("hosts").toArray();

    for (int i = 0; i < array.count(); i++)
    {
        HostInfo info;
        info.hostAddr = array.at(i)["hostAddr"].toString();
        info.userName = array.at(i)["userName"].toString();
        info.password = array.at(i)["password"].toString();
        info.rootPath = array.at(i)["rootPath"].toString();
        hostInfoVec.append(info);
        ui->hostList->addItem(info.hostAddr);
    }
}

void MainWindow::writeSettings()
{
    QJsonObject json;
    QJsonArray array;

    HostInfo info;

    info.hostAddr = ui->hostAddr->text() + ":" + QString::number(ui->port->value());
    info.password = ui->password->text();
    info.userName = ui->username->text();
    info.rootPath = ui->rootPath->text();

    hostInfoVec[ui->hostList->currentRow()] = info;
    ui->hostList->item(ui->hostList->currentRow())->setText(info.hostAddr);

    for (auto info : hostInfoVec)
    {
        QJsonObject value;
        value["hostAddr"] = info.hostAddr;
        value["userName"] = info.userName;
        value["password"] = info.password;
        value["rootPath"] = info.rootPath;

        array.append(value);
    }

    json["hosts"] = array;

    QFile file("settings.json");
    file.open(QIODevice::WriteOnly);
    QJsonDocument doc(json);

    file.write(doc.toJson());
    file.close();
}

void MainWindow::on_hostList_currentRowChanged(int currentRow)
{
    HostInfo info = hostInfoVec.at(currentRow);
    QStringList addr = info.hostAddr.split(":");
    if (addr.count() != 2) return;
    ui->hostAddr->setText(addr[0]);
    ui->port->setValue(addr[1].toInt());
    ui->username->setText(info.userName);
    ui->password->setText(info.password);
    ui->rootPath->setText(info.rootPath);
}

void MainWindow::on_actionAdd_triggered()
{

}

void MainWindow::on_add_clicked()
{
    HostInfo info;
    info.hostAddr = "new";

    hostInfoVec.append(info);
    ui->hostList->addItem(info.hostAddr);
    ui->hostList->setCurrentRow(ui->hostList->count() - 1);
}
