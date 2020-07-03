#ifndef FSTHREAD_H
#define FSTHREAD_H

#include <Winsock2.h>
#include <QThread>
#include <dokan/dokan.h>
#include "fsOperations.h"


class DokanyThread : public QThread
{
public:
    DokanyThread(QObject *parent = 0);
    ~DokanyThread();

protected:
    virtual void run();

private:
    FileSystem filenodes;

};

#endif // FSTHREAD_H
