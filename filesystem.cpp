#include "filesystem.h"
#include <dokanyThread.h>
#include <QDebug>

FileSystem::FileSystem(DokanyThread *dokany) : m_dokany(dokany), root(new fileNode)
{
    root->fileName = "/";
    root->isDirectory = true;
    fileIndexTable.insert("/", root);
}

void FileSystem::createFile(QString fileName, bool isDir, quint32 fileAttr, quint64 filesize)
{
    if (!fileIndexTable.contains(fileName))
    {
        fileNodePtr node(new fileNode);
        node->size = filesize;
        node->isDirectory = isDir;
        node->file_attr = fileAttr;
        fileIndexTable[fileName] = node;
        int pos = fileName.lastIndexOf("/");
        QString parentPath = fileName.left(pos + 1);
        node->setFileName(fileName.remove("/"));
        qDebug() << Q_FUNC_INFO << fileName << parentPath;
        if (fileIndexTable.contains(parentPath))
        {
            fileNodePtr &parentNode = fileIndexTable[parentPath];

            parentNode->children.append(node);
        }
    }
    else
    {
        fileNodePtr node = fileIndexTable[fileName];
        node->size = filesize;
        node->isDirectory = isDir;
        node->file_attr = fileAttr;
        node->setFileName(fileName.remove("\\"));
    }

}

QVector<fileNodePtr> FileSystem::listFolder(QString path)
{
    path = path.replace("\\", "/");


    m_dokany->getSshThread()->openDir(path);
    //qDebug() << Q_FUNC_INFO << path << fileIndexTable.value(path)->children;
    return fileIndexTable.value(path)->children;

}

fileNodePtr FileSystem::find(QString fileName)
{
    if (fileIndexTable.contains(fileName))
    {
        return fileIndexTable.value(fileName);
    }

    return nullptr;
}

void fileNode::setFileName(QString name)
{
    //  locker.lock();
    fileName = name;
    //  locker.unlock();
}
