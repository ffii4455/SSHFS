#include "filesystem.h"
#include <dokanyThread.h>
#include <QDebug>

FileSystem::FileSystem(DokanyThread *dokany, QString sshRootPath) : m_dokany(dokany), root(new FileNode),
    sshRootPath(sshRootPath)
{
    root->fileName = "/";
    root->isDirectory = true;
    fileIndexTable.insert("/", root);
}

void FileSystem::createFile(QString fileName, bool isDir, quint32 fileAttr, quint64 filesize)
{
    if (!fileIndexTable.contains(fileName))
    {
        FileNodePtr node(new FileNode);
        node->size = filesize;
        node->isDirectory = isDir;
        node->file_attr = fileAttr;
        fileIndexTable[fileName] = node;
        int pos = fileName.lastIndexOf("/");
        QString parentPath = fileName.left(pos + 1);
        node->setFileName(fileName.remove("/"));
       // qDebug() << Q_FUNC_INFO << fileName << parentPath;
        if (fileIndexTable.contains(parentPath))
        {
            FileNodePtr &parentNode = fileIndexTable[parentPath];

            parentNode->children.append(node);
        }
    }
    else
    {
        FileNodePtr node = fileIndexTable[fileName];
        node->size = filesize;
        node->isDirectory = isDir;
        node->file_attr = fileAttr;
        node->setFileName(fileName.remove("\\"));
    }

}

QVector<FileNodePtr> FileSystem::listFolder(QString path)
{
    path = path.replace("\\", "/");

    qDebug() << "listFolder " << path;


    return m_dokany->getSshThread()->openDir(sshRootPath + path);
}

FileNodePtr FileSystem::find(QString fileName)
{
    if (fileIndexTable.contains(fileName))
    {
        return fileIndexTable.value(fileName);
    }

    return nullptr;
}

void FileNode::setFileName(QString name)
{
    //  locker.lock();
    fileName = name;
    //  locker.unlock();
}

FileNode::FileNode(const FileNode &src)
{
    fileName = src.fileName;
    size = src.size;
}


