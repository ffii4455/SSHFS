#ifndef FSOPERATIONS_H
#define FSOPERATIONS_H

#include <winsock2.h>
#include <dokan/dokan.h>
#include "filesystem.h"

extern DOKAN_OPERATIONS memfs_operations;

#define GET_FS_INSTANCE \
  reinterpret_cast<FileSystem*>(dokanfileinfo->DokanOptions->GlobalContext)


#endif // FSOPERATIONS_H
