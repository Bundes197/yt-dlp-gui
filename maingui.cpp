#include "maingui.h"
#include "./ui_maingui.h"
#include <QFileDialog>
#include <QDir>

MainGUI::MainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
{
    ui->setupUi(this);
}

MainGUI::~MainGUI()
{
    delete ui;
}

void MainGUI::on_directoryButton_clicked()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, "Select destination directory", QDir::homePath());
    ui->pathPrint->setText(directoryPath);
}

