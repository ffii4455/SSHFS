#include "fsOperations.h"
#include <QDebug>
#include "filesystem.h"


fileSystem fs;

static NTSTATUS DOKAN_CALLBACK
memfs_createfile(LPCWSTR filename, PDOKAN_IO_SECURITY_CONTEXT security_context,
                 ACCESS_MASK desiredaccess, ULONG fileattributes,
                 ULONG /*shareaccess*/, ULONG createdisposition,
                 ULONG createoptions, PDOKAN_FILE_INFO dokanfileinfo)
{
    ACCESS_MASK generic_desiredaccess;
    DWORD creation_disposition;
    DWORD file_attributes_and_flags;

    DokanMapKernelToUserCreateFileFlags(
                desiredaccess, fileattributes, createoptions, createdisposition,
                &generic_desiredaccess, &file_attributes_and_flags,
                &creation_disposition);

    qDebug() << "CreateFile" << QString::fromStdWString(filename) << createoptions << creation_disposition;

    auto filename_str = std::wstring(filename);
    auto f = fs.find(QString::fromStdWString(filename_str));

    if (filename_str == L"\\System Volume Information" ||
            filename_str == L"\\$RECYCLE.BIN") {
        return STATUS_NO_SUCH_FILE;
    }

    if (f && f->isDirectory)
    {
        if (createoptions & FILE_NON_DIRECTORY_FILE)
            return STATUS_FILE_IS_A_DIRECTORY;
        dokanfileinfo->IsDirectory = true;
    }

    if (dokanfileinfo->IsDirectory)
    {
        if (creation_disposition == CREATE_NEW ||
                creation_disposition == OPEN_ALWAYS)
        {
            if (f)
                return STATUS_OBJECT_NAME_COLLISION;

           fs.createFile(QString::fromStdWString(filename), true, FILE_ATTRIBUTE_DIRECTORY);
           return STATUS_SUCCESS;
        }

        if (f && !f->isDirectory)
            return STATUS_NOT_A_DIRECTORY;
        if (!f)
            return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    switch (creation_disposition)
    {
        case CREATE_ALWAYS:
        {
            fs.createFile(QString::fromStdWString(filename), false, file_attributes_and_flags);
            break;
        }
        case CREATE_NEW:
        {
            if (f) return STATUS_OBJECT_NAME_COLLISION;
            fs.createFile(QString::fromStdWString(filename), false, file_attributes_and_flags);
            break;
        }

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

    for (auto file : fs.listFolder(QString::fromStdWString(filename)))
    {
        auto name = file->fileName.toStdWString();
        std::copy(name.begin(), name.end(), std::begin(findData.cFileName));
        findData.cFileName[name.length()] = '\0';
        findData.nFileSizeLow = file->size;
        findData.dwFileAttributes = file->file_attr;
        fill_finddata(&findData, dokanfileinfo);
    }
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

