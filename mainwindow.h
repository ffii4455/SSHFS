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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startConnect_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<fsThread> fs;
    std::unique_ptr<sshThread> ss;
};
#endif // MAINWINDOW_H
