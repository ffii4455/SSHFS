#ifndef FSTHREAD_H
#define FSTHREAD_H

#include <Winsock2.h>
#include <QThread>
#include <dokan/dokan.h>
#include "fsOperations.h"
#include "sshthread.h"
#include <QDebug>

struct HostInfo
{
    QString hostAddr;
    QString userName;
    QString password;
    QString rootPath;
};


class DokanyThread : public QThread
{
public:
    DokanyThread(const HostInfo &host, QObject *parent = 0);
    ~DokanyThread();

    FileSystem* getFilenodes() {return &filenodes;}
    SshThread*  getSshThread() {return &sshThread;}

protected:
    virtual void run();

private:
    FileSystem filenodes;
    SshThread  sshThread;
    HostInfo   m_hostInfo;

};

#endif // FSTHREAD_H
