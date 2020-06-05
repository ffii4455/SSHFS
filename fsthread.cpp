#include "fsthread.h"

fsThread::fsThread(QObject *parent) : QThread(parent)
{

}

fsThread::~fsThread()
{
    DokanRemoveMountPoint(L"m:");
    exit();
    wait();

}

void fsThread::run()
{
    DOKAN_OPTIONS dokan_options;
    ZeroMemory(&dokan_options, sizeof(DOKAN_OPTIONS));
    dokan_options.Version = DOKAN_VERSION;
    dokan_options.Options = DOKAN_OPTION_ALT_STREAM;
    //dokan_options.MountPoint = mount_point;

   // dokan_options.Options |= DOKAN_OPTION_STDERR | DOKAN_OPTION_DEBUG;

    // Mount type

    dokan_options.Options |= DOKAN_OPTION_NETWORK;

    dokan_options.ThreadCount = 1;
    dokan_options.Timeout = 30;
    dokan_options.GlobalContext = 1234;

    dokan_options.MountPoint = L"m:";

    NTSTATUS status = DokanMain(&dokan_options, &memfs_operations);
    switch (status)
    {
        case DOKAN_SUCCESS:
            break;
        case DOKAN_ERROR:
        {
            qDebug("Error");
            break;
        }
        case DOKAN_DRIVE_LETTER_ERROR:
        {
            qDebug("Bad Drive letter");
            break;
        }
        case DOKAN_DRIVER_INSTALL_ERROR:
        {
            qDebug("Can't install driver");
            break;
        }
        case DOKAN_START_ERROR:
        {
            qDebug("Driver something wrong");
            break;
        }
        case DOKAN_MOUNT_ERROR:
        {
            qDebug("Can't assign a drive letter");
            break;
        }
        case DOKAN_MOUNT_POINT_ERROR:
        {
            qDebug("Mount point error");
            break;
        }
        case DOKAN_VERSION_ERROR:
        {
            qDebug("Version error");
            break;
        }
        default:
        {
            qDebug("Unknown error"); // add error status
        }
    }


}
