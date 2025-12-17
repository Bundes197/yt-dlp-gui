#include "maingui.h"
#include "./ui_maingui.h"
#include <QFileDialog>
#include <QDir>
#include <QProcess>
// #include <QMessageBox>

MainGUI::MainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
    , process(nullptr)
{
    ui->setupUi(this);

    // set error labels invisible by default
    ui->urlErrorLabel->hide();
    ui->pathErrorLabel->hide();

    ui->urlErrorLabel->setStyleSheet("QLabel { color : FireBrick; }");
    ui->pathErrorLabel->setStyleSheet("QLabel { color : FireBrick; }");
}

MainGUI::~MainGUI()
{
    if (process) {
        process->kill();
        delete process;
    }

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
    // reset labels
    ui->urlErrorLabel->hide();
    ui->pathErrorLabel->hide();

    QString url = ui->urlInput->text();
    QString directoryPath = ui->pathPrint->text();
    bool flag = false;

    if (url.isEmpty())
    {
        flag = true;
        ui->urlErrorLabel->show();
    }

    if (directoryPath.isEmpty())
    {
        flag = true;
        ui->pathErrorLabel->show();
    }

    if (flag) return;

    ui->urlErrorLabel->hide();
    ui->pathErrorLabel->hide();

    if (process) {
        process->kill();
        delete process;
    }

    process = new QProcess(this);

    connect(process, &QProcess::finished, this, &MainGUI::onProcessFinished);

    process->setWorkingDirectory(directoryPath);

    QString command = "/opt/homebrew/bin/yt-dlp";
    QString ffmpegLocation = "/opt/homebrew/bin/ffmpeg";

    QStringList args;
    args << "-x"
         << "--audio-format" << "mp3"
         << "--ffmpeg-location" << ffmpegLocation
         << "-P" << directoryPath
         << url;

    ui->status->setStyleSheet("QLabel { color : gold; }");
    ui->status->setText("Starting download...");

    process->start(command, args);

    ui->status->setText("Downloading...");

    ui->downloadButton->setEnabled(false);
    ui->downloadButton->setText("Downloading...");
}

void MainGUI::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->downloadButton->setEnabled(true);
    ui->downloadButton->setText("Download");

    ui->status->setStyleSheet("QLabel { color : LawnGreen; }");
    ui->status->setText("Download finished!");

    // if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    //     ui->stdout->show();
    //     ui->stdout->setText("Download completed successfully!");
    //     QMessageBox::information(this, "Success", "Download completed!");
    // } else {
    //     ui->stderr->show();
    //     QString errorMsg = process->readAllStandardError();
    //     ui->stderr->setText("Error: " + errorMsg);
    //     QMessageBox::warning(this, "Error", "Download failed!\nExit code: " + QString::number(exitCode));
    // }
}
