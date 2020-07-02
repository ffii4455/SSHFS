#include "filesystem.h"
#include <QDebug>

FileSystem::FileSystem() : root(new fileNode)
{
    root->fileName = "\\";
    root->isDirectory = true;
    fileIndexTable.insert("\\", root);
}

void FileSystem::createFile(QString fileName, bool isDir, quint32 fileAttr, quint64 filesize)
{
  //  qDebug() << Q_FUNC_INFO << fileName;

    if (!fileIndexTable.contains(fileName))
    {
        fileNodePtr node(new fileNode);
        node->size = filesize;
        node->isDirectory = isDir;
        node->file_attr = fileAttr;
        fileIndexTable[fileName] = node;
        int pos = fileName.lastIndexOf("\\");
        QString parentPath = fileName.left(pos + 1);
        node->setFileName(fileName.remove("\\"));

        fileNodePtr &parentNode = fileIndexTable[parentPath];
        parentNode->children.append(node);
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
    qDebug() << "listFolder" << path;
    if (fileIndexTable.contains(path))
    {
        emit openDir(path);
        return fileIndexTable.value(path)->children;
    }
    else
    {
        emit openDir(path);
        return QVector<fileNodePtr>();
    }
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
