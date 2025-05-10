///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright @ NetworkDLS 2002, All rights reserved
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <QFile>
#include <QByteArray>
#include <QCRC32.H>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QCRC32::QCRC32(QObject *parent) : QObject(parent) {
    Initialize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QCRC32::~QCRC32() {
    // No destructor code
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	This function initializes "CRC Lookup Table". You only need to call it once to
		initalize the table before using any of the other CRC32 calculation functions.
*/
void QCRC32::Initialize() {
    // Initialize CRC Lookup Table
    unsigned int iPolynomial = 0x04C11DB7;
    memset(iTable, 0, sizeof(iTable));

    for (int iCodes = 0; iCodes <= 0xFF; iCodes++) {
        iTable[iCodes] = Reflect(iCodes, 8) << 24;
        for (int iPos = 0; iPos < 8; iPos++) {
            iTable[iCodes] = (iTable[iCodes] << 1) ^ ((iTable[iCodes] & (1 << 31)) ? iPolynomial : 0);
        }
        iTable[iCodes] = Reflect(iTable[iCodes], 32);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Reflection is a requirement for the official CRC-32 standard.
	You can create CRCs without it, but they won't conform to the standard.
*/

unsigned int QCRC32::Reflect(unsigned int iReflect, const char cChar) {
    unsigned int iValue = 0;

    for (int iPos = 1; iPos < (cChar + 1); iPos++) {
        if (iReflect & 1) {
            iValue |= (1 << (cChar - iPos));
        }
        iReflect >>= 1;
    }
    return iValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Calculates the CRC32 by looping through each of the bytes in sData.
	
	Note: For Example usage example, see FileCRC().
*/
void QCRC32::PartialCRC(quint32 *iCRC, const QByteArray &data) {
    for (int i = 0; i < data.size(); ++i) {
        *iCRC = (*iCRC >> 8) ^ iTable[(*iCRC & 0xFF) ^ static_cast<unsigned char>(data.at(i))];
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Returns the calculated CRC32 (through iOutCRC) for the given string.
*/
void QCRC32::FullCRC(const QByteArray &data, quint32 *iOutCRC) {
    *iOutCRC = 0xffffffff; // Initialize the CRC
    PartialCRC(iOutCRC, data);
    *iOutCRC ^= 0xffffffff; // Finalize the CRC and return
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Returns the calculated CRC23 for the given string.
*/
quint32 QCRC32::FullCRC(const QByteArray &data) {
    quint32 iCRC = 0xffffffff; // Initialize the CRC
    PartialCRC(&iCRC, data);
    return iCRC ^ 0xffffffff; // Finalize the CRC and return
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Calculates the CRC32 of a file using the a user defined buffer.

	Note: The buffer size DOES NOT affect the resulting CRC,
			it has been provided for performance purposes only.
*/

bool QCRC32::FileCRC(const QString &fileName, quint32 *iOutCRC, size_t bufferSize) {
    *iOutCRC = 0xffffffff; // Initialize the CRC
    QFile file(fileName);
    if (!file.exists()) {
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        return false; // Failed to open file for read access
    }

    QByteArray buffer;
    buffer.resize(bufferSize);
    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer.data(), buffer.size());
        PartialCRC(iOutCRC, buffer.left(bytesRead));
    }

    file.close();
    *iOutCRC ^= 0xffffffff; // Finalize the CRC
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Calculates the CRC32 of a file using the a default buffer size of 1MB.
*/
quint32 QCRC32::FileCRC(const QString &fileName) {
    quint32 iCRC;
    if (FileCRC(fileName, &iCRC, 1048576)) {
        return iCRC;
    }
    return 0xffffffff; // Return error code
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Calculates the CRC32 of a file using the a default buffer size of 1MB.

	Note: The buffer size DOES NOT affect the resulting CRC,
			it has been provided for performance purposes only.
*/
quint32 QCRC32::FileCRC(const QString &fileName, size_t bufferSize) {
    quint32 iCRC;
    if (FileCRC(fileName, &iCRC, bufferSize)) {
        return iCRC;
    }
    return 0xffffffff; // Return error code
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	Calculates the CRC32 of a file using the a default buffer size of 1MB.
*/
bool QCRC32::FileCRC(const QString &fileName, quint32 *iOutCRC) {
    return this->FileCRC(fileName, iOutCRC, 1048576);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
