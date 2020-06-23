#include "filesystem.h"

fileSystem::fileSystem() : root(new fileNode)
{
    root->fileName = "\\";
    root->isDirectory = true;
    fileIndexTable.insert("\\", root);
}

void fileSystem::createFile(QString fileName, bool isDir, quint32 fileAttr)
{
    fileNodePtr node(new fileNode);

    node->isDirectory = isDir;
    node->file_attr = fileAttr;
    fileIndexTable[fileName] = node;
    int pos = fileName.lastIndexOf("\\");
    QString parentPath = fileName.left(pos + 1);
    node->setFileName(fileName.remove("\\"));

    fileNodePtr &parentNode = fileIndexTable[parentPath];
    parentNode->children.append(node);
}

QVector<fileNodePtr> fileSystem::listFolder(QString path)
{
    if (fileIndexTable.contains(path))
    {
        return fileIndexTable.value(path)->children;
    }
    else
    {
        return QVector<fileNodePtr>();
    }
}

fileNodePtr fileSystem::find(QString fileName)
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
