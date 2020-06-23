#ifndef FSTHREAD_H
#define FSTHREAD_H

#include <Winsock2.h>
#include <QThread>
#include <dokan/dokan.h>
#include "fsOperations.h"


class fsThread : public QThread
{
public:
    fsThread(QObject *parent = 0);
    ~fsThread();

protected:
    virtual void run();
};

#endif // FSTHREAD_H
