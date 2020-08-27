#include "sshthread.h"
#include <QDebug>
#include "fsOperations.h"
#include "dokanyThread.h"
#include <QtConcurrent>

#define GET_FILE_NODES (m_dokany->getFilenodes())

SshThread::SshThread(DokanyThread *dokany, QObject *parent) :
    m_dokany(dokany),
    QObject(parent)
{

}

SshThread::~SshThread()
{
    if (sftp_session) libssh2_sftp_shutdown(sftp_session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

void SshThread::start()
{
    QtConcurrent::run(this, &SshThread::initSSH);
}

void SshThread::setSshPara(QString hostaddr, int port, QString username, QString password, QString rootPath)
{
    this->hostaddr = inet_addr(hostaddr.toLocal8Bit().data());
    this->port = port;
    this->username = username;
    this->password = password;
    this->rootPath = rootPath;

    this->rootPath.replace("\\", "/");
    if (this->rootPath.endsWith("/")) this->rootPath.chop(1);
}

void SshThread::_openDir(QString path)
{
    //  locker.lock();

    LIBSSH2_SFTP_HANDLE *sftp_handle;
    int rc;
    if (sftp_session == NULL)
    {
        qDebug() << "sftp handle or session is invalid!!";
        return;
    }

    if (!path.endsWith("/")) path.append("/");

    qDebug() << "SSH open dir" << rootPath + path;
    sftp_handle = libssh2_sftp_opendir(sftp_session, (rootPath + path).toLocal8Bit().data());

    if(!sftp_handle) {
        qDebug() << "Unable to open dir with SFTP" << sftp_handle;
        return;
    }
    qDebug() << "libssh2_sftp_opendir() is done, now receive listing!\n";
    do {
        char mem[512];
        char longentry[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;

        /* loop until we fail */
        rc = libssh2_sftp_readdir_ex(sftp_handle, mem, sizeof(mem),
                                     longentry, sizeof(longentry), &attrs);
        if(rc > 0) {
            /* rc is the length of the file name in the mem
                   buffer */

            //qDebug() << QString::fromLocal8Bit(mem) << attrs.permissions << attrs.filesize << attrs.atime << attrs.mtime;
            if ((attrs.permissions >> 12) == 0x4)
            {
                GET_FILE_NODES->createFile(QString("%1%2").arg(path).arg(mem), true, FILE_ATTRIBUTE_DIRECTORY, attrs.filesize);
            }
            else
            {
                GET_FILE_NODES->createFile(QString("%1%2").arg(path).arg(mem), false, 0, attrs.filesize);
            }


            if(longentry[0] != '\0') {
                // printf("##%s##\n", longentry);
            }
            else {
                if(attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) {
                    /* this should check what permissions it
                           is and print the output accordingly */
                    printf("--fix----- ");
                }
                else {
                    printf("---------- ");
                }

                if(attrs.flags & LIBSSH2_SFTP_ATTR_UIDGID) {
                    printf("%4d %4d ", (int) attrs.uid, (int) attrs.gid);
                }
                else {
                    printf("   -    - ");
                }

                if(attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
                    printf("%8" __FILESIZE " ", attrs.filesize);
                }

                //  printf("%s\n", mem);
            }
        }
        else
            break;

    } while(1);

    libssh2_sftp_closedir(sftp_handle);
    // locker.unlock();
}

void SshThread::openDir(QString path)
{
    _openDir(path.replace("\\", "/"));
}

void SshThread::initSSH()
{
    qDebug() << "ssh start init";
    int rc, i, auth_pw = 0;
    const char *fingerprint;
    char *userauthlist;

    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if(err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return;
    }

    rc = libssh2_init(0);
    if(rc != 0) {
        fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = hostaddr;
    if(::connect(sock, (struct sockaddr*)(&sin),
                 sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return;
    }

    session = libssh2_session_init();
    if(!session)
        return;

    /* ... start it up. This will trade welcome banners, exchange keys,
         * and setup crypto, compression, and MAC layers
         */
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
         * is check the hostkey's fingerprint against our known hosts Your app
         * may have it hard coded, may go to a file, may present it to the
         * user, that's your call
         */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    /* check what authentication methods are available */
    userauthlist = libssh2_userauth_list(session, username.toLocal8Bit().data(), username.length());
    fprintf(stderr, "Authentication methods: %s\n", userauthlist);
    if(strstr(userauthlist, "password") != NULL) {
        auth_pw |= 1;
    }
    if(strstr(userauthlist, "keyboard-interactive") != NULL) {
        auth_pw |= 2;
    }
    if(strstr(userauthlist, "publickey") != NULL) {
        auth_pw |= 4;
    }

    if(auth_pw & 1)
    {
        /* We could authenticate via password */
        if(libssh2_userauth_password(session, username.toLocal8Bit().data(), password.toLocal8Bit().data()))
        {
            fprintf(stderr, "\tAuthentication by password failed!\n");
            goto shutdown;
        }
        else {
            fprintf(stderr, "\tAuthentication by password succeeded.\n");
        }
    }

    fprintf(stderr, "libssh2_sftp_init()!\n");
    sftp_session = libssh2_sftp_init(session);

    if(!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
        goto shutdown;
    }

    qDebug() << "ssh start init success";

    /* Since we have not set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(session, 1);

    // openDir(rootPath);

    fprintf(stderr, "libssh2_sftp_opendir()!\n");
    /* Request a dir listing via SFTP */

    // libssh2_sftp_shutdown(sftp_session);

shutdown:

    //libssh2_session_disconnect(session, "Normal Shutdown");
    //libssh2_session_free(session);


    fprintf(stderr, "all done\n");

    //  libssh2_exit();
}
