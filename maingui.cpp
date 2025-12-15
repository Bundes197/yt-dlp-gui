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
    if (!directoryPath.isEmpty())
    {
        ui->pathPrint->setText(directoryPath);
    }
}



void MainGUI::on_downloadButton_clicked()
{
    QString url = ui->urlInput->text();
    QString directoryPath = ui->pathPrint->text();
    bool flag = false;

    if (url.isEmpty())
    {
        flag = true;
    }

    if (directoryPath.isEmpty())
    {
        flag = true;
    }

    if (flag) return;
}

