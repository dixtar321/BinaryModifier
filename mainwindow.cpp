#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(664, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileOps;
    delete timer;
}

/**
 * @brief MainWindow::on_deleteCheckBox_toggled - if user will choose delete origin files - overwrite file will remove, else - add overwrite
 * @param checked - bool param for deleteCheckBox
 */
void MainWindow::on_deleteCheckBox_toggled(bool checked)
{
    qDebug() << "Delete input file option:" << checked;
    if(checked){
        ui->fileExistComboBox->removeItem(1);
    }
    else{
        ui->fileExistComboBox->addItem("Overwrite the file", QVariant(0));
    }
}

/**
 * @brief MainWindow::on_oneTimeLaunchComboBox_toggled - if user will choose run program one time - timer will hide, else - timer will show
 * @param checked - bool param for oneTimeLaunchComboBox
 */
void MainWindow::on_oneTimeLaunchComboBox_toggled(bool checked)
{
    qDebug() << "One-time launch option:" << checked;
    if(checked){
        ui->frequencyLabel->hide();
        ui->frequencyTimeEdit->hide();
    }
    else{
        ui->frequencyLabel->show();
        ui->frequencyTimeEdit->show();
    }

}

/**
 * @brief MainWindow::on_modifyButton_clicked - func for final button (modifier)
 */
void MainWindow::on_modifyButton_clicked()
{
    QString fileMask = ui->maskComboBox->currentText();
    QString savePath = ui->savePathLine->text();
    bool deleteInput = ui->deleteCheckBox->isChecked();
    bool overwrite = (ui->fileExistComboBox->currentIndex() == 1);
    QByteArray xorValue = ui->lineEdit->text().toUtf8();
    bool oneTimeLaunch = ui->oneTimeLaunchComboBox->isChecked();

    // XOR empty check
    if (xorValue.isEmpty()) {
        qDebug() << "XOR value is empty. Please enter a valid XOR value.";
        return;
    }

    // check path
    QDir dir(savePath);
    if (!dir.exists()) {
        qDebug() << "Save path does not exist. Please enter a valid path.";
        return;
    }

    if (fileOps == nullptr) {
        fileOps = new FileOperations(fileMask, savePath, deleteInput, overwrite, xorValue, oneTimeLaunch);
    }

    if (oneTimeLaunch) {
        fileOps->modifyFiles();  // One-Time launch program
    } else { //timer launch
        int interval = ui->frequencyTimeEdit->time().msecsSinceStartOfDay();
        if (interval <= 0) {
            qDebug() << "Invalid timer interval. It must be greater than 0.";
            return;  // Возврат, если интервал некорректен
        }

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [&]() {
            try {
                fileOps->modifyFiles();
            } catch (const std::exception& e) {
                qDebug() << "Error during file modification:" << e.what();
            }
        });
        connect(this, &MainWindow::destroyed, timer, &QTimer::stop);  // Остановить таймер при закрытии окна
        timer->start(interval);
    }
}


void MainWindow::on_browseButton_clicked() //func for browse button, which need to select directory
{
    // Открываем диалог для выбора директории
    QString dirName = QFileDialog::getExistingDirectory(this,
                                                        tr("Select Directory"),
                                                        "",
                                                        QFileDialog::ShowDirsOnly);

    // check directory (variable) empty
    if (dirName.isEmpty()) {
        qDebug() << "No directory selected!";
        return;
    }

    // check directory exists
    QDir selectedDir(dirName);
    if (!selectedDir.exists()) {
        qDebug() << "Selected directory does not exist:" << dirName;
        return;
    }

    // set directory path in Path Line
    ui->savePathLine->setText(dirName);
    qDebug() << "Path set to QLineEdit:" << ui->savePathLine->text();
}


