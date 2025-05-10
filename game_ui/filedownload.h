#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include "define.h"
#include <qthread.h>
#include <QNetworkSession>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork>


QT_BEGIN_NAMESPACE
class GameLauncher;
namespace Ui { class GameLauncher; }
QT_END_NAMESPACE

class FileDownload : public QThread
{
    Q_OBJECT
public:
    explicit FileDownload(QObject *parent, Ui::GameLauncher *ui);
    ~FileDownload();
    void run();    
    unsigned int DownloadFile();
public slots:
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError err);
    void finished();

private:
    GameLauncher *game;
    Ui::GameLauncher *gameUI;
    QNetworkAccessManager _netManager;
    QNetworkRequest _request;
    QNetworkReply* _reply;
    QFile* _file;

    void EndUpdate();
};

#endif // FILEDOWNLOAD_H
