#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Winsock2.h>
#include <QMainWindow>
#include <memory>

#include "fsthread.h"
#include "sshthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct HostInfo
    {
        QString hostAddr;
        QString userName;
        QString password;
        QString rootPath;
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startConnect_clicked();

    void on_saveButton_clicked();

    void on_hostList_currentRowChanged(int currentRow);

private:
    Ui::MainWindow *ui;
    std::shared_ptr<DokanyThread> fs;
    std::shared_ptr<SshThread> ss;

    QVector<HostInfo> hostInfoVec;
    void readSettings();
    void writeSettings();
};
#endif // MAINWINDOW_H
