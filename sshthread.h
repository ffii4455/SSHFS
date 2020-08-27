#ifndef SSHTHREAD_H
#define SSHTHREAD_H

#include <winsock2.h>

#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>
#include "filesystem.h"

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
#include <QMutex>

class dokanyThread;

class SshThread : public QObject
{
    Q_OBJECT
public:
    SshThread(DokanyThread *dokany, QObject *parent = 0);
    ~SshThread();
    void start();
    void setSshPara(QString hostaddr, int port, QString username, QString password, QString rootPath);

private:
    unsigned long hostaddr, port;
    QString username, password, rootPath;

    LIBSSH2_SFTP *sftp_session = nullptr;
    LIBSSH2_SESSION *session = nullptr;
    WSADATA wsadata;
    struct sockaddr_in sin;
    int sock = -1;

private:
    // void initSession
    void initSSH();
    void _openDir(QString path);
    QMutex locker;

    DokanyThread *m_dokany;

public slots:
    void openDir(QString path);

};

#endif // SSHTHREAD_H
