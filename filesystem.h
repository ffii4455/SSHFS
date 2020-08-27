#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <Winsock2.h>
#include <QString>
#include <QVector>
#include <QHash>
#include <memory>
#include <QMutex>
#include <QObject>

struct filetimes
{
  std::atomic<quint64> creation = 0;
  std::atomic<quint64> lastaccess = 0;
  std::atomic<quint64> lastwrite = 0;
};

struct fileNode
{    
    QString fileName = "";
    bool isDirectory = false;
    quint32 file_attr = 0;
    quint64 size = 0;
    filetimes times;
    QVector<std::shared_ptr<fileNode>> children;
    void setFileName(QString name);

    fileNode() = default;
};

typedef std::shared_ptr<fileNode> fileNodePtr;

class DokanyThread;

class FileSystem : public QObject
{
    Q_OBJECT
public:
    FileSystem(DokanyThread* dokany);
    void createFile(QString fileName, bool isDir, quint32 fileAttr, quint64 filesize);
    QVector<fileNodePtr> listFolder(QString path);
    fileNodePtr find(QString fileName);
private:
    fileNodePtr root;
    QHash<QString, fileNodePtr> fileIndexTable;
    DokanyThread* m_dokany;

signals:
    void openDir(QString path);
};

#endif // FILESYSTEM_H
