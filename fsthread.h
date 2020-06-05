#ifndef FSTHREAD_H
#define FSTHREAD_H

#include <QThread>
#include <dokan/dokan.h>
#include "fsOperations.h"


class fsThread : public QThread
{
public:
    fsThread(QObject *parent);
    ~fsThread();

protected:
    virtual void run();
};

#endif // FSTHREAD_H
