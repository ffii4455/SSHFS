#include "fsOperations.h"
#include <QDebug>
#include <QThread>

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


    auto filenodes = GET_FS_INSTANCE;



    auto filename_str = std::wstring(filename);
    auto f = filenodes->find(QString::fromStdWString(filename_str));

    //  qDebug() << "memfs_createfile " << filename_str;

    if (filename_str == L"\\System Volume Information" ||
        filename_str == L"\\$RECYCLE.BIN")
    {
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

            filenodes->createFile(QString::fromStdWString(filename), true, FILE_ATTRIBUTE_DIRECTORY, 0);
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
        // filenodes->createFile(QString::fromStdWString(filename), false, file_attributes_and_flags, 0);
        break;
    }
    case CREATE_NEW:
    {
        if (f) return STATUS_OBJECT_NAME_COLLISION;
        //  filenodes->createFile(QString::fromStdWString(filename), false, file_attributes_and_flags, 0);
        break;
    }

    }

    //  qDebug() << "memfs_createfile SUCCESS";
    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK memfs_findfiles(LPCWSTR filename,
                                               PFillFindData fill_finddata,
                                               PDOKAN_FILE_INFO dokanfileinfo)
{
    qDebug() << "FindFile" << QString::fromStdWString(filename);
    WIN32_FIND_DATAW findData;
    ZeroMemory(&findData, sizeof(WIN32_FIND_DATAW));

    auto filenodes = GET_FS_INSTANCE;

    for (auto file : filenodes->listFolder(QString::fromStdWString(filename)))
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

static void DOKAN_CALLBACK memfs_cleanup(LPCWSTR filename,
                                         PDOKAN_FILE_INFO dokanfileinfo)
{

}

static void DOKAN_CALLBACK memfs_closeFile(LPCWSTR filename,
                                           PDOKAN_FILE_INFO /*dokanfileinfo*/)
{

}

static NTSTATUS DOKAN_CALLBACK memfs_readfile(LPCWSTR filename, LPVOID buffer,
                                              DWORD bufferlength,
                                              LPDWORD readlength,
                                              LONGLONG offset,
                                              PDOKAN_FILE_INFO dokanfileinfo)
{
    auto filenodes = GET_FS_INSTANCE;
    auto filename_str = std::wstring(filename);
    //    auto f = filenodes->find(filename_str);
    //    if (!f)
    //        return STATUS_OBJECT_NAME_NOT_FOUND;

    qDebug() << "memfs_readfile " <<  QString::fromStdWString(filename);

    QByteArray b;

    if (filenodes->readFile(QString::fromStdWString(filename_str), b, offset) < 0)
    {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    if (b.length() > bufferlength) b.resize(bufferlength);

    *readlength = b.length();

    memcpy(buffer, b.data(), *readlength);

    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK memfs_getdiskfreespace(
    PULONGLONG free_bytes_available, PULONGLONG total_number_of_bytes,
    PULONGLONG total_number_of_free_bytes, PDOKAN_FILE_INFO dokanfileinfo)
{
    *free_bytes_available = (ULONGLONG)(512 * 1024 * 1024);
    *total_number_of_bytes = MAXLONGLONG;
    *total_number_of_free_bytes = MAXLONGLONG;
    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK memfs_getvolumeinformation(
    LPWSTR volumename_buffer, DWORD volumename_size,
    LPDWORD volume_serialnumber, LPDWORD maximum_component_length,
    LPDWORD filesystem_flags, LPWSTR filesystem_name_buffer,
    DWORD filesystem_name_size, PDOKAN_FILE_INFO /*dokanfileinfo*/)
{

    wcscpy_s(volumename_buffer, volumename_size, L"Dokan MemFS");
    *volume_serialnumber = 123456;
    *maximum_component_length = 255;
    *filesystem_flags = FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES |
                        FILE_SUPPORTS_REMOTE_STORAGE | FILE_UNICODE_ON_DISK |
                        FILE_NAMED_STREAMS;

    wcscpy_s(filesystem_name_buffer, filesystem_name_size, L"NTFS");
    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK
memfs_getfileInformation(LPCWSTR filename, LPBY_HANDLE_FILE_INFORMATION buffer,
                         PDOKAN_FILE_INFO dokanfileinfo) {
    auto filenodes = GET_FS_INSTANCE;
    auto filename_str = std::wstring(filename);


    buffer->nNumberOfLinks = 1;
    buffer->dwVolumeSerialNumber = 123456;

    auto strLength = 100;

    LlongToDwLowHigh(strLength, buffer->nFileSizeLow,
                     buffer->nFileSizeHigh);

    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK memfs_writefile(LPCWSTR filename, LPCVOID buffer,
                                               DWORD number_of_bytes_to_write,
                                               LPDWORD number_of_bytes_written,
                                               LONGLONG offset,
                                               PDOKAN_FILE_INFO dokanfileinfo) {

    return STATUS_SUCCESS;
}



DOKAN_OPERATIONS memfs_operations = {memfs_createfile,
                                     memfs_cleanup,
                                     memfs_closeFile,
                                     memfs_readfile,
                                     memfs_writefile,
                                     nullptr,
                                     memfs_getfileInformation,
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
                                     memfs_getdiskfreespace,
                                     memfs_getvolumeinformation,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr};

