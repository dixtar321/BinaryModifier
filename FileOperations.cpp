#include "fileoperations.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QLockFile>
#include <QMessageBox>


FileOperations::FileOperations(const QString& mask, const QString& outputDir, bool removeOriginal, bool overwrite, const QString& xorVal, bool oneTimeLaunch, bool usingHex)
    : fileMask(mask), outputDirectory(outputDir), removeOriginalFiles(removeOriginal), overwriteExistingFiles(overwrite), xorValue(xorVal), oneTimeLaunch(oneTimeLaunch), useHexValue(usingHex){}


bool FileOperations::foolCheck(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(nullptr, "Error", "The file could not be opened. \nIt is possible that the file has already been opened by another application.");
        return false;
    }

    file.close();
    return true;
}

QByteArray FileOperations::hexToBytes(const QString &str){
    QByteArray bytes;
    bool justForCheck; // variable just for checking result in func toUInt(bool, int)
    for (int i = 0; i < str.size(); i += 2) {
        // Reading every two symbols as one byte in HEX format
        bytes.append(static_cast<char>(str.mid(i, 2).toUInt(&justForCheck, 16)));
    }
    return bytes;
}

void FileOperations::modifyFiles() {

    if (outputDirectory.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "Output directory is empty!");
        return;
    }

    QDir dir(outputDirectory);
    if (!dir.exists()) {
        QMessageBox::warning(nullptr, "Error", "Directory " + outputDirectory +" does not exist:");
        return;
    }

    if (!dir.exists()) {
        QMessageBox::warning(nullptr, "Error", "Input directory " + outputDirectory +" does not exist:");
        return;
    }

    QStringList files = dir.entryList({fileMask}, QDir::Files);

    qDebug() << "Files found:" << files;

    foreach (const QString& file, files) {
        QString filePath = dir.absoluteFilePath(file);
        if(foolCheck(filePath)){
            modifyFile(filePath);
        }
    }
}

/**
 * @brief FileOperations::modifyFile - second func for modify files.
 * @param filePath
 */

void FileOperations::modifyFile(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", "Cannot open file for reading: " + filePath);
        return;
    }

    // checking if XOR is empty
    if (xorValue.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "XOR value is empty. Skipping file modification.");
        return;
    }

    QByteArray fileData = file.readAll();
    qDebug() << "fileData (before XOR):" << fileData.toHex();
    file.close();

    QByteArray xorValueByte;

    if (useHexValue) {
        xorValueByte = hexToBytes(xorValue);
        qDebug() << "Using hex value for XOR";
    } else {
        xorValueByte = xorValue.toUtf8();
        qDebug() << "Using ASCII value for XOR";
    }

    qDebug() << "xorValue:" << xorValueByte.toHex();

    // Проверка на пустой массив
    if (xorValueByte.isEmpty()) {
        qDebug() << "xorValueByte is empty!";
        return; // Выход, если массив пуст
    }

    // XOR модификация
    for (int i = 0; i < fileData.size(); ++i) {
        fileData[i] = fileData[i] ^ xorValueByte[i % xorValueByte.size()];
    }

    qDebug() << "fileData after XOR:" << fileData.toHex();


    qDebug() << "fileData after XOR:" << fileData.toHex();

    // result path
    QDir outDir(outputDirectory);
    if (!outDir.exists()) {
        outDir.mkpath(".");  // if dir is not exist - it will create
    }

    QString outputFilePath = outDir.absoluteFilePath(QFileInfo(filePath).fileName()); // for _modified files. It not used when overwriting is True.
    if (!overwriteExistingFiles && QFile::exists(outputFilePath)) {
        outputFilePath = outDir.absoluteFilePath(QFileInfo(filePath).baseName() + "_modified." + QFileInfo(filePath).suffix());
    }

    QFile outputFile(outputFilePath); //check file for write
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing:" << outputFilePath;
        return;
    }

    outputFile.write(fileData);
    outputFile.close();
    qDebug() << "Modified file saved to:" << outputFilePath;

    if (removeOriginalFiles) { // It is using when remove original files is True
        QFile::remove(filePath);
        qDebug() << "Original file deleted:" << filePath;
    }
}
