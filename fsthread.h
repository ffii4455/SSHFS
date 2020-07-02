#ifndef FSTHREAD_H
#define FSTHREAD_H

#include <Winsock2.h>
#include <QThread>
#include <dokan/dokan.h>
#include "fsOperations.h"


class FsThread : public QThread
{
public:
    FsThread(QObject *parent = 0);
    ~FsThread();

protected:
    virtual void run();

private:

};

#endif // FSTHREAD_H
