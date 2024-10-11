#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "FileOperations.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_deleteCheckBox_toggled(bool checked); //func that delete choise "Overwrite the file" if user select "Delete original" or or does it do the opposite
    void on_oneTimeLaunchComboBox_toggled(bool checked); // func that timer if user select "One-Time launch"
    void on_modifyButton_clicked(); //modify button (the required fields must be filled in)
    void on_browseButton_clicked(); //button for find directory

private:
    Ui::MainWindow *ui;
    QTimer *timer = nullptr; // at the beginning timer is nullptr
    FileOperations *fileOps = nullptr; // at the beginning fileOps is nullptr
};

#endif // MAINWINDOW_H
