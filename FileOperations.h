#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QString>

/**
 * @brief The FileOperations class that class include methods and params for work Binary Modifier
 */

class FileOperations {
    /**
 * @brief FileOperations - contructor of class.
 * @param mask - can be .txt or .bat
 * @param outputPath - directory path that includes input files. Also output files will save in that directory.
 * @param removeOriginal - bool param. True - the original files are deleted, modified ones appear. False - there are 2 types of files left
 * @param overwrite - bool param. True - original files will overwrite. False - creating modified files
 * @param xorValue - value that is involved in the bitwise xor operation with files
 * @param oneTimeLaunch - bool param. True - program working without timer, just 1 time. False - program working with timer.
 */
public:
    FileOperations(const QString& mask, const QString& outputPath, bool removeOriginal,
                   bool overwrite, const QString& xorValue, bool oneTimeLaunch, bool useHexValue);
    /**
     * @brief modifyFiles - main function for modifying files
     */
    void modifyFiles();

private:
    QString fileMask;
    QString outputDirectory;
    bool removeOriginalFiles;
    bool overwriteExistingFiles;
    QString xorValue;
    bool oneTimeLaunch;
    bool useHexValue;

    /**
     * @brief modifyFile - main func for modify files
     * @param filePath
     */
    void modifyFile(const QString& filePath);
    /**
     * @brief foolCheck - func for checking file access/
     * @param filePath
     * @return - return true if file not opened another program (like MS Word), else - return false
     */
    bool foolCheck(const QString &filePath);
    /**
     * @brief hexToBytes - func for convert hex to Bytes
     * @param str - string from user input
     * @return
     */
    QByteArray hexToBytes(const QString &str);

};

#endif // FILEOPERATIONS_H
