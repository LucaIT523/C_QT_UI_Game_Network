///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright @ NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
#ifndef QCRC32_H
#define QCRC32_H

#include <QObject>

class QCRC32 : public QObject
{
    Q_OBJECT

public:
    explicit QCRC32(QObject *parent = nullptr);
    ~QCRC32();

    void Initialize();
    bool FileCRC(const QString &fileName, quint32 *outCRC);
    bool FileCRC(const QString &fileName, quint32 *outCRC, size_t bufferSize);
    quint32 FileCRC(const QString &fileName);
    quint32 FileCRC(const QString &fileName, size_t bufferSize);
    quint32 FullCRC(const QByteArray &data);
    void FullCRC(const QByteArray &data, quint32 *outCRC);
    void PartialCRC(quint32 *crc, const QByteArray &data);

private:
    quint32 Reflect(quint32 reflect, char cChar);
    quint32 iTable[256]; // CRC lookup table array.
};

#endif // QCRC32_H
