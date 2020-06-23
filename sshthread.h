#ifndef SSHTHREAD_H
#define SSHTHREAD_H

#include <winsock2.h>

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif


#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#define __FILESIZE "I64"
#else
#define __FILESIZE "llu"
#endif

#include <QThread>

class sshThread : public QThread
{
    Q_OBJECT
public:
    sshThread(QObject *parent = 0);
    void setSshPara(QString hostaddr, int port, QString username, QString password, QString rootPath);

protected:
    virtual void run();

private:
    unsigned long hostaddr, port;
    QString username, password, rootPath;

private slots:
   // void initSession

};

#endif // SSHTHREAD_H
