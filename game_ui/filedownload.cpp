#include "filedownload.h"
#include <QDebug>
#include "QCRC32.H"
#include "gamelauncher.h"
#include "ui_gamelauncher.h"

FileDownload::FileDownload( QObject *parent, Ui::GameLauncher *ui):QThread(parent)
{
    game = (GameLauncher*)parent;
    gameUI = ui;
}
FileDownload::~FileDownload()
{
    _reply->deleteLater();
    _file->close();
    this->quit();
    this->wait();
}
void FileDownload::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        QByteArray b = _reply->readAll();
        QDataStream out(_file);
        out << b;
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        gameUI->progressBar->setValue(percentage);
    }
}
void FileDownload::error(QNetworkReply::NetworkError err)
{
    // Manage error here.
    qDebug() << QString(err);
    _reply->deleteLater();
    _file->close();
}
void FileDownload::finished()
{
    // Save the image here
    _reply->deleteLater();
    _file->close();
    // done
}
unsigned int FileDownload::DownloadFile()
{
    QByteArray filebuf;
    gameUI->stateLabel->setText("Đang lấy thông tin bản cập nhật mới...");// Set status bar text

    _request.setUrl(QUrl(game->strURL));
    _reply = _netManager.get(_request);
     _file= new QFile(QFileInfo(game->strURL).fileName()); // "des" is the file path to the destination file
     _file->open(QIODevice::WriteOnly);
    /*QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();*/


    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    connect(_reply, SIGNAL(finished()), this, SLOT(finished()));
    while(_reply->isFinished()){
        sleep(1);
    }
     while(_reply->isRunning()){
         sleep(1);
     }
   /*
    * connect(reply, &QNetworkReply::downloadProgress, this, &Launcher::updateProgress);
    * connect(reply, &QNetworkReply::finished, [=]() {
       if (reply->error() != QNetworkReply::NoError) {
           QMessageBox::critical(nullptr, "Error", "Failed to download file");
           gameUI->progressBar->reset();
       } else {
           // Download completed successfully
           reply->deleteLater(); // Clean up the reply object
       }
   });

   if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::critical(nullptr, "Error", "Failed to download file");
        gameUI->progressBar->reset();
        return 0;
    }
   reply->deleteLater();*/
    return 1;
}
void FileDownload::run()
{
    qDebug() << "Files Updating...";
    gameUI->startButton->setEnabled(false);
    gameUI->groupBox->setEnabled(false);

    QCRC32 crc32;
    unsigned int iCRC = 0;
    QFile myReadFile(VERSION_FILE_NAME);
    QString strstep;
    if (!myReadFile.open(QIODevice::ReadOnly))
    {
       myReadFile.open(QIODevice::WriteOnly);
       myReadFile.write("0");
       myReadFile.close();
       strstep = "0";
    }
    else
    {
       strstep = QString(myReadFile.readAll());
       myReadFile.close();
    }
    if (game->strVersioninfo.size() > strstep.toInt())
    {
        for (int i = strstep.toInt(); i < game->strVersioninfo.count(); i++)
        {
            QString strdown = game->strVersioninfo.at(i);
            QString strURL = QString(SERVER_MAIN_DATA).arg(strdown);
            game->nTerminate = 0;
            // Begin download process
            DownloadFile(); // You need to implement the download logic

            QFile out(VERSION_FILE_NAME);
            out.open(QIODevice::WriteOnly);
            out.write(QString::number(i + 1).toUtf8());
            out.close();
        }
    }
    int nfilenumber = 0;
    for (int i = 0; i < 1/*strImportantinfo.size()/3*/; i++)
    {
        QString strfilename, strCrc, strDownfilename;
        strfilename = game->strImportantinfo.at(i*3);
        strCrc = game->strImportantinfo.at(i*3+1);
        strDownfilename = game->strImportantinfo.at(i*3+2);

        // This is check CRC List
        gameUI->stateLabel->setText("Đang kiểm tra các file cần thiết. Vui lòng chờ...");
        if (crc32.FileCRC(strfilename, &iCRC))
        {
            QString str;
            str=QStringLiteral("%1").arg(iCRC, 8, 16, QLatin1Char('0'));
            if (str.toUpper() != strCrc)
            {
                nfilenumber++;
                game->strURL = QString(SERVER_MAIN_DATA).arg(strDownfilename);
                game->nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
                DownloadFile();
            }

        }
        else
        {
            nfilenumber++;
            game->strURL = QString(SERVER_MAIN_DATA).arg(strDownfilename);
            game->nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
            DownloadFile();
        }
    }
    QString strServerCRC32, strparameter;
    strServerCRC32 = game->strServerinfo.at((game->nServerNo-1)*4 +1);
    strparameter = game->strServerinfo.at((game->nServerNo-1)*4 +2);
    game->strURL = QString(SERVER_MAIN_DATA).arg((QString)game->strServerinfo.at((game->nServerNo-1)*4 +3));

    if (crc32.FileCRC(MAIN_CRC32, &iCRC))
    {
        if (game->strMainCRC.toUpper() != strServerCRC32)
        {
            game->nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
            // Begin our download thread
            //DownloadFile();
        }
    }
    else
    {
        game->nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
        // Begin our download thread
        //DownloadFile();
    }
    qDebug() << "File update has been completed.";
    QString strCommand;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 now = currentDateTime.toSecsSinceEpoch();
    now += 1283; // Add 1283 to the current time

    QByteArray pTimeHash = QByteArray::number(now);
    strCommand = QString("client.exe %1 %2").arg(strparameter).arg(QString::fromLatin1(pTimeHash));


    QProcess process;
    process.startDetached(strCommand);

    if (process.waitForStarted()) {
        // Process started successfully
        qDebug() << "Process started successfully.";
        EndUpdate();
        exit(0);
    } else {
        // Process failed to start
        qDebug() << "Failed to start the process.";
        EndUpdate();
    }
}
void FileDownload::EndUpdate()
{
  qDebug() << "quit thread";
  this->quit();
  this->wait();
  gameUI->startButton->setEnabled(true);
  gameUI->groupBox->setEnabled(true);
}
