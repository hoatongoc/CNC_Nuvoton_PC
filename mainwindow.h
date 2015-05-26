#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "motor_control.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void InitializeCommands(QStringList& cList, QString sourceText);
    std::vector<GCode> ProcessCommand();
    ~MainWindow();

private slots:

    void on_connBtn_clicked();

    void on_dirBtn_clicked();

    void on_runBtn_clicked();

    void on_stepBtn_clicked();

    void on_speedBtn_clicked();

    void on_statusBtn_clicked();

    void updateGUI();

    void on_OpenButton_clicked();

    void on_pushButton_clicked();

    void on_runDrawBtn_clicked();

    void on_ZRatioBtn_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
