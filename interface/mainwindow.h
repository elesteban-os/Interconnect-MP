#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_uploadButton2_clicked();

    void on_uploadButton3_clicked();

    void on_uploadButton4_clicked();

    void on_uploadButton5_clicked();

    void on_uploadButton6_clicked();

    void on_uploadButton7_clicked();

    void on_uploadButton8_clicked();

    void on_uploadButton1_clicked();

    void on_PEButton1_clicked();

    void on_PEButton2_clicked();

    void on_PEButton3_clicked();

    void on_PEButton4_clicked();

    void on_PEButton5_clicked();

    void on_PEButton6_clicked();

    void on_PEButton7_clicked();

    void on_PEButton8_clicked();

    void on_ICButton_clicked();

    void on_SMButton_clicked();

    void on_GRAPHICButton_clicked();

    void on_GRAPHICButton2_clicked();

    void on_LSButton_clicked();

    void on_LSButton2_clicked();

    void on_LSButton3_clicked();

    void on_LSButton4_clicked();

    void on_LSButton5_clicked();

    void on_LSButton6_clicked();

    void on_LSButton7_clicked();

    void on_LSButton8_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
