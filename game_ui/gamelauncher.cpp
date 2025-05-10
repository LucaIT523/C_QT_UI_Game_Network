#include "define.h"
#include "gamelauncher.h"
#include "ui_gamelauncher.h"
#include <QBitmap>
#include <QPixmap>
#include <qpushbutton.h>
#include <QMessageBox>

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

#include <qscrollarea.h>
#include <QVBoxLayout>
#include <QAxWidget>

#include "QCRC32.H"

GameLauncher::GameLauncher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameLauncher)
{
    ui->setupUi(this);

    this->serverMap.insert(1,ui->sv1);ui->sv1->hide();
    this->serverMap.insert(2,ui->sv2);ui->sv2->hide();
    this->serverMap.insert(3,ui->sv3);ui->sv3->hide();
    this->serverMap.insert(4,ui->sv4);ui->sv4->hide();
    this->serverMap.insert(5,ui->sv5);ui->sv5->hide();
    this->serverMap.insert(6,ui->sv6);ui->sv6->hide();
    this->serverMap.insert(7,ui->sv7);ui->sv7->hide();
    this->serverMap.insert(8,ui->sv8);ui->sv8->hide();
    this->serverMap.insert(9,ui->sv9);ui->sv9->hide();
    connect(ui->sv1,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv2,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv3,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv5,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv6,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv7,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));
    connect(ui->sv8,SIGNAL(clicked()),this,SLOT(on_serverBtn_clicked()));

    ui->ieWidget->setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
    ui->ieWidget->dynamicCall("Navigate(const QString&)", "http://rdsa.thucsonkyhiep3d.com/feed");

// QScrollArea* scrollArea = new QScrollArea(this);
//   scrollArea->setWidget(ui->groupBox);

//    QVBoxLayout *Layout = new QVBoxLayout;
//    ui->groupBox ->setLayout(Layout);
//    QPushButton* s1 = new QPushButton(this);
//    Layout->addWidget(s1);

    /*
    QPixmap normalStart(":/res/ui2_btn_start_normal.png");
    QPixmap hoverStart(":/res/ui2_btn_start_wait.png");
    QPixmap downStart(":/res/ui2_btn_start_down.png");
    QPixmap disableStart(":/res/ui2_btn_start_disable.png");
    DrawableButton launcherButton(normalStart, hoverStart, downStart, disableStart, this);
    launcherButton.setGeometry(100, 100, 150, 150);
    this->startButton->setStyleSheet( "*{border-image: url(:/res/ui2_btn_start_normal.png);}"
                                                 ":pressed{ border-image: url(:/res/ui2_btn_start_down.png);}");
    */
    /*
    background-image: url(:/res/back.png);
    background-repeat: no-repeat;
    background-position: center;
     */
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPixmap pixmap(":/res/back.png");
    QBitmap mask = pixmap.createHeuristicMask(true);
    this->setMask(mask);
    /*
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPixmap pixmap(":/res/back.png");

    // Create a mask based on the transparency of the image
    QBitmap mask = pixmap.createHeuristicMask(false);
    this->setMask(mask);
    this->centralWidget()->setStyleSheet("background-image:url(:/res/back.png);background-repeat: no-repeat;");
*/
    OnInit();
}

GameLauncher::~GameLauncher()
{
    delete ui;
}

void GameLauncher::OnInit(){
    bDownState = false;
    nServerNo = 0;
    QCRC32 crc32;
    //crc32.Initialize();
    unsigned int iCRC = 0;
    ui->progressBar->reset();
    ui->stateLabel->setText("Vui lòng nhấp vào \"vào trò chơi\"--->>");
    if(crc32.FileCRC(MAIN_CRC32, &iCRC)){
        strMainCRC=QStringLiteral("%1").arg(iCRC, 8, 16, QLatin1Char('0'));
    }

    QNetworkAccessManager manager;
    //Get Server List.
    QNetworkReply *serverReply = manager.get(QNetworkRequest(QUrl(SERVER_LIST_URL)));

    QEventLoop loop;
    QObject::connect(serverReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray info;
    info = serverReply->readAll();
    serverReply->deleteLater();
    strServerinfo = info.split('\"');
    for (int i = strServerinfo.size() - 1; i >= 0; --i) {
        if (strServerinfo.at(i) == "," || strServerinfo.at(i) == "" || strServerinfo.at(i) == "\r\n" ) {
            strServerinfo.removeAt(i);
        }
    }
    QPushButton* pServerBtn;
    for (int n = 0; n < strServerinfo.size()/4; n++)
    {
        if(n >= this->serverMap.size())
            break;
        pServerBtn = this->serverMap.value(n+1);
        pServerBtn->setText(strServerinfo.at(n*4));
        pServerBtn->show();
        if (strMainCRC.toUpper() == strServerinfo.at(n*4+1))
        {
            nServerNo = n + 1;
            pServerBtn->setChecked(true);
        }
    }

    //const QChar* qcharData = reinterpret_cast<const QChar*>(data.constData());
    QFile file(QFileInfo(SERVER_LIST_URL).fileName());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(info);
        file.close();
    }//debug

    //Get Important Data
    QNetworkReply *importantReply = manager.get(QNetworkRequest(QUrl(IMPORTANT_LIST_URL)));
    QEventLoop importantLoop;
    QObject::connect(importantReply, &QNetworkReply::finished, &importantLoop, &QEventLoop::quit);
    importantLoop.exec();
    info = importantReply->readAll();
    importantReply->deleteLater();
    strImportantinfo = info.split('\"');
    for (int i = strImportantinfo.size() - 1; i >= 0; --i) {
        if (strImportantinfo.at(i) == "," || strImportantinfo.at(i) == "" || strImportantinfo.at(i) == "\r\n" ) {
            strImportantinfo.removeAt(i);
        }
    }

    file.setFileName(QFileInfo(IMPORTANT_LIST_URL).fileName());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
     file.write(info);
     file.close();
    }//debug

    // Get Update Version List
    QNetworkReply *versionReply = manager.get(QNetworkRequest(QUrl(UPDATE_VERSION_LIST_URL)));
    QEventLoop versionLoop;
    QObject::connect(versionReply, &QNetworkReply::finished, &versionLoop, &QEventLoop::quit);
    versionLoop.exec();
    info = versionReply->readAll();
    versionReply->deleteLater();
    strVersioninfo = info.split('\"');
    for (int i = strVersioninfo.size() - 1; i >= 0; --i) {
        if (strVersioninfo.at(i) == "," || strVersioninfo.at(i) == "" || strVersioninfo.at(i) == "\r\n" ) {
            strVersioninfo.removeAt(i);
        }
    }
    //strVersioninfo = QString::fromUtf8(versionData);
    file.setFileName(QFileInfo(UPDATE_VERSION_LIST_URL).fileName());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
     file.write(info);
     file.close();
    }//debug
}
void GameLauncher::error(QNetworkReply::NetworkError err)
{
    // Manage error here.
    qDebug() << QString(err);
    _reply->deleteLater();
    _file->close();    
    bDownState = false;
    ui->stateLabel->setText("Tải về cập nhật đã bị hủy bỏ...");
    ui->startButton->setEnabled(true);
    ui->groupBox->setEnabled(true);
    emit downloadFinished();
}
void GameLauncher::finished()
{
    // Save the image here
    _reply->deleteLater();
    _file->close();
    qDebug() << "finished download file!"<<fileName;
    unzip7zFile(fileName);
    emit downloadFinished();
    // done
}
void GameLauncher::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        if (nTerminate == 1) // See if we got a Cancel requesst
        {
            _reply->deleteLater();// Close remote file
            _file->close(); // Close our destination file            
            bDownState = false;
            ui->stateLabel->setText("Tải về cập nhật đã bị hủy bỏ...");// Set status bar text
            ui->startButton->setEnabled(true);
            ui->groupBox->setEnabled(true);
            ui->progressBar->reset();
            emit downloadFinished();
            return;
        }

        QByteArray b = _reply->readAll();
        _file->write(b.constData(), b.size());

        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        ui->progressBar->setValue(percentage);

        QDateTime dlCurrent = QDateTime::currentDateTime(); // Current time
        qint64 secs = dlStartDownload.secsTo(dlCurrent); // Calculate total seconds between start of download up until now

        // User info calculations
        int nperc = static_cast<int>((bytesReceived * 100) / bytesTotal); // Calculate percentage
        unsigned long kbrecv = bytesReceived / 1024; // Get received KB
        double kbsec = static_cast<double>(kbrecv) / secs; // Get KB download per second

        QString Perc = QString::number(nperc); // Format percent into string
        QString KBin = QString::number(kbrecv); // Format read KB into string
        QString FileSize = QString::number(bytesTotal/1024); // Format read KB into string
        QString KBsec = QString::number(kbsec, 'f', 1); // Format KB/sec into string

        // Update GUI and all the good stuff
        ui->stateLabel->setText("Downloading... " + fName + "(" + Perc + "%) " + KBin + "kb/" + FileSize + "kb(" + KBsec + "Kb/sec)");

    }
}
void GameLauncher::unzip7zFile(QString filePath)
{
    QProcess process;
    QString program = QString("7-Zip/7z.exe x %1 -aoa").arg(filePath);

    ui->stateLabel->setText("Tải về tập tin cập nhật hoàn tất");
    try {
        ui->stateLabel->setText("Đang cập nhật dữ liệu game...");
        qDebug() << "Start 7z process!";
        process.start(program);
        if (!process.waitForStarted(-1)) {
            qDebug() << "Error starting 7z process:" << process.errorString();
        } else {
            qDebug() << "Start Extraction!"<<fileName;
            if (!process.waitForFinished(-1)) {
                qDebug() << "7z process Error:" << process.errorString();
            } else {
                if (process.exitStatus() == QProcess::NormalExit) {
                    qDebug() << "Extraction successful!"<<fileName;
                    _file->remove();
                } else {
                    qDebug() << "Error extracting file."<<fileName<< "-Exit code:" << process.exitCode();
                }
            }
        }
        ui->stateLabel->setText("Dữ liệu game đã cập nhật hoàn tất");
        ui->stateLabel->setText("Cập nhật phiên bản đã hoàn tất");
    }
    catch(const QException &ex)
    {
        exit(1);
    }

}
void GameLauncher::DownloadFile()
{    
    QByteArray filebuf;
    ui->stateLabel->setText("Đang lấy thông tin bản cập nhật mới...");// Set status bar text

    _request.setUrl(QUrl(strURL));
    _reply = _netManager.get(_request);

    fileName = QFileInfo(strURL).fileName();
    _file= new QFile(fileName); // "des" is the file path to the destination file
    _file->open(QIODevice::WriteOnly);
    qDebug() << "Start download file!"<<fileName;

    fName= fileName;
     // Get our new name!
    if (fileName.size() >= 10)
    {
        fName = fileName.mid(0, 7); // Cut it up
        fName = fName + "..."; // Add 3 dots "..."
    }

    dlStartDownload = QDateTime::currentDateTime(); // Start time of download

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    connect(_reply, SIGNAL(finished()), this, SLOT(finished()));
}
void GameLauncher::UpdateFile(){
    bDownState = true;
    qDebug() << "Files Updating...";

    ui->startButton->setEnabled(false);
    ui->groupBox->setEnabled(false);

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
    if (strVersioninfo.size() > strstep.toInt())
    {
        for (int i = strstep.toInt(); i < strVersioninfo.count(); i++)
        {
            QString strdown = strVersioninfo.at(i);
            strURL = QString(SERVER_MAIN_DATA).arg(strdown);
            nTerminate = 0;
            // Begin download process
            DownloadFile(); // You need to implement the download logic
            waiting_update_finish();
            if(bDownState == false){
                return;
            }
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
        strfilename = strImportantinfo.at(i*3);
        strCrc = strImportantinfo.at(i*3+1);
        strDownfilename = strImportantinfo.at(i*3+2);

        // This is check CRC List
        ui->stateLabel->setText("Đang kiểm tra các file cần thiết. Vui lòng chờ...");
        if (crc32.FileCRC(strfilename, &iCRC))
        {
            QString str;
            str=QStringLiteral("%1").arg(iCRC, 8, 16, QLatin1Char('0'));
            if (str.toUpper() != strCrc)
            {
                nfilenumber++;
                strURL = QString(SERVER_MAIN_DATA).arg(strDownfilename);
                nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
                DownloadFile();
                waiting_update_finish();
                if(bDownState == false){
                    return;
                }
            }

        }
        else
        {
            nfilenumber++;
            strURL = QString(SERVER_MAIN_DATA).arg(strDownfilename);
            nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
            DownloadFile();
            waiting_update_finish();
            if(bDownState == false){
                return;
            }
        }
    }
    QString strServerCRC32, strparameter;
    strServerCRC32 = strServerinfo.at((nServerNo-1)*4 +1);
    strparameter = strServerinfo.at((nServerNo-1)*4 +2);
    strURL = QString(SERVER_MAIN_DATA).arg((QString)strServerinfo.at((nServerNo-1)*4 +3));

    if (crc32.FileCRC(MAIN_CRC32, &iCRC))
    {
        if (strMainCRC.toUpper() != strServerCRC32)
        {
            nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
            // Begin our download thread
            DownloadFile();
            waiting_update_finish();
            if(bDownState == false){
                return;
            }
        }
    }
    else
    {
        nTerminate = 0; // Clear the Terminate value to 0 (starting a new download)
        // Begin our download thread
        DownloadFile();
        waiting_update_finish();
        if(bDownState == false){
            return;
        }
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
        ui->stateLabel->setText("Bản cập nhật đã được tải xuống thành công.");
        exit(0);
    } else {
        // Process failed to start
        qDebug() << "Failed to start the process.";
//        ui->stateLabel->setText("Cập nhật thành công (100%). Không thể bắt đầu quá trình.");
//        ui->startButton->setEnabled(true);
//        ui->groupBox->setEnabled(true);
//        bDownState = false;
    }
    exit(0);
}
void GameLauncher::waiting_update_finish()
{
    QEventLoop loop;
    connect(this, &GameLauncher::downloadFinished, &loop, &QEventLoop::quit);
    loop.exec();
}
void GameLauncher::on_startButton_clicked()
{
    if (nServerNo == 0){
        QMessageBox:: warning ( this, "game", "Vui lòng chọn máy chủ để vào game") ;
        //QMessageBox::Question, QMessageBox::Information, QMessageBox::Warning, MessageBox::Critical(https://qthelper.ru/qmessagebox/)
    }
    else
    {
        //ui->startButton->setEnabled(false);
        //CWinThread * pThread = AfxBeginThread(UpdateFile, GetSafeHwnd(), THREAD_PRIORITY_NORMAL);
        UpdateFile();
    }
}

void GameLauncher::on_closeButton_clicked()
{
    if (bDownState)
    {
        if (QMessageBox::question(this, "kiểm tra", "Hủy bỏ cập nhật?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            nTerminate = 1;
        }
    }
    else
        this->close();
}

void GameLauncher::on_minButton_clicked()
{
    this->showMinimized();
}

void GameLauncher::on_serverBtn_clicked()
{
    QPushButton *pServerBtn = qobject_cast<QPushButton *>(sender());
    nServerNo = this->serverMap.key(pServerBtn);
}
