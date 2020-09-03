#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <Winsock2.h>
#include <QString>
#include <QVector>
#include <QHash>
#include <memory>
#include <QMutex>
#include <QObject>

static inline LONGLONG DDwLowHighToLlong(const DWORD& low,
                                         const DWORD& high) {
    return static_cast<LONGLONG>(high) << 32 | low;
}

static inline void LlongToDwLowHigh(const LONGLONG& v, DWORD& low,
                                    DWORD& hight) {
    hight = v >> 32;
    low = static_cast<DWORD>(v);
}

static inline LONGLONG FileTimeToLlong(const FILETIME& f) {
    return DDwLowHighToLlong(f.dwLowDateTime, f.dwHighDateTime);
}

static inline void LlongToFileTime(LONGLONG v, FILETIME& filetime) {
    LlongToDwLowHigh(v, filetime.dwLowDateTime, filetime.dwHighDateTime);
}



struct filetimes
{
  std::atomic<quint64> creation = 0;
  std::atomic<quint64> lastaccess = 0;
  std::atomic<quint64> lastwrite = 0;
};

struct FileNode
{
    QString fileName = "";
    bool isDirectory = false;
    quint32 file_attr = 0;
    quint64 size = 0;
    filetimes times;
    QVector<std::shared_ptr<FileNode>> children;
    void setFileName(QString name);

    FileNode() = default;
    FileNode(const FileNode &src);
    FileNode(FileNode &&src) = default;

};

typedef std::shared_ptr<FileNode> FileNodePtr;

class DokanyThread;

class FileSystem : public QObject
{
    Q_OBJECT
public:
    FileSystem(DokanyThread* dokany, QString sshRootPath);
    void createFile(QString fileName, bool isDir, quint32 fileAttr, quint64 filesize);
    QVector<FileNodePtr> listFolder(QString path);
    FileNodePtr find(QString fileName);
    int readFile(QString path, QByteArray &buffer, int offset);
private:
    FileNodePtr root;
    QHash<QString, FileNodePtr> fileIndexTable;
    DokanyThread* m_dokany;
    QString       sshRootPath;

signals:
    void openDir(QString path);
};

#endif // FILESYSTEM_H
