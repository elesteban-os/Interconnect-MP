#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTextEditStream.h"
#include "../SystemSimulator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

QTextEditStream* textEditBuffer;
std::streambuf* oldCoutStream;
SystemSimulator sim;

void redirectOutputToTextEdit(QTextEdit* edit) {
    textEditBuffer = new QTextEditStream(edit);
    oldCoutStream = std::cout.rdbuf(textEditBuffer);
}

void executeSystem() {

    sim.start();

    //while(sim.step()) {}

    //sim.waitForThreads();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //redirectOutputToTextEdit(ui->textEdit);
    executeSystem();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_uploadButton1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton4_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}
void MainWindow::on_uploadButton5_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton6_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton7_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_uploadButton8_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Selected", "You selected:\n" + fileName);
    }
}

void MainWindow::on_PEButton1_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton1->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton2_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton2->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton3_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton3->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton4_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton4->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton5_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton5->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton6_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton6->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton7_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton7->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_PEButton8_clicked()
{
        // Cambiar color del botón a verde
        ui->PEButton8->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_ICButton_clicked()
{
        // Cambiar color del botón a verde
        ui->ICButton->setStyleSheet("background-color: green; color: white;");
}

void MainWindow::on_SMButton_clicked()
{
        // Cambiar color del botón a verde
        ui->SMButton->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_GRAPHICButton_clicked()
{
        // Cambiar color del botón a verde
        ui->GRAPHICButton->setStyleSheet("background-color: green; color: white;");

}
void MainWindow::on_GRAPHICButton2_clicked()
{
        // Cambiar color del botón a verde
        ui->GRAPHICButton2->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton2_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton2->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton3_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton3->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton4_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton4->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton5_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton5->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton6_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton6->setStyleSheet("background-color: green; color: white;");

}
void MainWindow::on_LSButton7_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton7->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_LSButton8_clicked()
{
        // Cambiar color del botón a verde
        ui->LSButton->setStyleSheet("background-color: green; color: white;");

}

void MainWindow::on_nextButton_clicked()
{
    sim.step();
}

