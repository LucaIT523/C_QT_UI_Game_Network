#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>

#include <QMap>
#include <QPushButton>

#include <QNetworkSession>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class GameLauncher; }
QT_END_NAMESPACE

class GameLauncher : public QMainWindow
{
    Q_OBJECT

public:
    int nServerNo;
    int nTerminate = 0; // Cancelling Download variable
    int nDownloaded = 0; // Downloaded complete verifier
    QString strURL;
    bool bDownState;
    int nStepnumber;
    int nindexversionlist[200];
    int nindexserverlist[80];
    int nindexfilelist[60];
    QList<QByteArray> strServerinfo;
    QList<QByteArray> strImportantinfo;
    QList<QByteArray> strVersioninfo;
    QString strMainCRC;
    QNetworkAccessManager _netManager;
    QNetworkRequest _request;
    QNetworkReply* _reply;
    QFile* _file;

    GameLauncher(QWidget *parent = nullptr);
    ~GameLauncher();
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            isDragging = true;
            offset = event->globalPos() - pos();
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton && isDragging) {
            move(event->globalPos() - offset);
        }
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            isDragging = false;
        }
    }
public: signals:
    void downloadFinished();
private slots:
    void on_startButton_clicked();
    void on_closeButton_clicked();
    void on_minButton_clicked();

    void on_serverBtn_clicked();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError err);
    void finished();

private:
    QMap<int, QPushButton*> serverMap;
    void OnInit();
    void waiting_update_finish();
    void UpdateFile();
    void DownloadFile();
    void unzip7zFile(QString filePath);

    Ui::GameLauncher *ui;
    bool isDragging;
    QPoint offset;
    QDateTime dlStartDownload;
    QString fileName;
    QString fName;
};
#endif // GAMELAUNCHER_H
