#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Winsock2.h>
#include <QMainWindow>
#include <memory>

#include "dokanyThread.h"
#include "sshthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT   

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startConnect_clicked();

    void on_saveButton_clicked();

    void on_hostList_currentRowChanged(int currentRow);

    void on_actionAdd_triggered();

    void on_add_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<DokanyThread> dokanyThread;
    std::shared_ptr<SshThread> ss;

    QVector<HostInfo> hostInfoVec;
    void readSettings();
    void writeSettings();
};
#endif // MAINWINDOW_H
