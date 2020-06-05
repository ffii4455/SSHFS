#include "fsOperations.h"
#include <QDebug>

static NTSTATUS DOKAN_CALLBACK
memfs_createfile(LPCWSTR filename, PDOKAN_IO_SECURITY_CONTEXT security_context,
                 ACCESS_MASK desiredaccess, ULONG fileattributes,
                 ULONG /*shareaccess*/, ULONG createdisposition,
                 ULONG createoptions, PDOKAN_FILE_INFO dokanfileinfo)
{
    qDebug() << "CreateFile" << QString::fromStdWString(filename) << createdisposition;

    if (QString::fromStdWString(filename) == "\\456")
    {
        qDebug() << "DIR";
        if (createoptions & FILE_NON_DIRECTORY_FILE)
        {
            return STATUS_FILE_IS_A_DIRECTORY;
        }

        dokanfileinfo->IsDirectory = true;


    }

    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK memfs_findfiles(LPCWSTR filename,
                                               PFillFindData fill_finddata,
                                               PDOKAN_FILE_INFO dokanfileinfo)
{
    qDebug() << "FindFile" << QString::fromStdWString(filename);
    WIN32_FIND_DATAW findData;
    ZeroMemory(&findData, sizeof(WIN32_FIND_DATAW));
    auto name = QString("123").toStdWString();
    std::copy(name.begin(), name.end(), std::begin(findData.cFileName));
    findData.cFileName[name.length()] = '\0';
    findData.nFileSizeLow = 1000;

    fill_finddata(&findData, dokanfileinfo);

    name = QString("456").toStdWString();
    std::copy(name.begin(), name.end(), std::begin(findData.cFileName));
    findData.cFileName[name.length()] = '\0';
    findData.nFileSizeLow = 0;
    findData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    fill_finddata(&findData, dokanfileinfo);
    return STATUS_SUCCESS;
}

DOKAN_OPERATIONS memfs_operations = {memfs_createfile,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     memfs_findfiles,
                                     nullptr,  // FindFilesWithPattern
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr};

