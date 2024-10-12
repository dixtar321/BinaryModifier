#include "fileoperations.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QLockFile>
#include <QMessageBox>


FileOperations::FileOperations(const QString& mask, const QString& outputDir, bool removeOriginal, bool overwrite, const QByteArray& xorVal, bool oneTimeLaunch)
    : fileMask(mask), outputDirectory(outputDir), removeOriginalFiles(removeOriginal), overwriteExistingFiles(overwrite), xorValue(xorVal), oneTimeLaunch(oneTimeLaunch){}


bool FileOperations::foolCheck(const QString &filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(nullptr, "Error", "The file could not be opened. \nIt is possible that the file has already been opened by another application.");
        return false;
    }

    file.close();
    return true;
}

void FileOperations::modifyFiles() {

    if (outputDirectory.isEmpty()) {
        qDebug() << "Output directory is empty!";
        return;
    }

    QDir dir(outputDirectory);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << outputDirectory;
        return;
    }

    if (!dir.exists()) {
        qDebug() << "Input directory does not exist:" << outputDirectory;
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
        qDebug() << "Cannot open file for reading:" << filePath;
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    // checking if XOR is empty
    if (xorValue.isEmpty()) {
        qDebug() << "XOR value is empty. Skipping file modification.";
        return;
    }

    // xor modification
    for (int i = 0; i < fileData.size(); ++i) {
        fileData[i] = fileData[i] ^ xorValue[i % xorValue.size()];
    }

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
