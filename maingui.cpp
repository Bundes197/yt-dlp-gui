#include "maingui.h"
#include "./ui_maingui.h"
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegularExpression>

MainGUI::MainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
    , process(nullptr) {
    ui->setupUi(this);

    // set error labels invisible by default
    ui->urlErrorLabel->hide();
    ui->pathErrorLabel->hide();

    ui->urlErrorLabel->setStyleSheet("QLabel { color : FireBrick; }");
    ui->pathErrorLabel->setStyleSheet("QLabel { color : FireBrick; }");

    detectBinaries();
}

void MainGUI::detectBinaries() {
    // find needed binaries, disable download if not found
    ytdlpPath = QStandardPaths::findExecutable("yt-dlp");
    ytdlpPath = QStandardPaths::findExecutable("yt-dlp", {"/opt/homebrew/bin"});

    if (ytdlpPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "yt-dlp not found!");
        ui->downloadButton->setEnabled(false);
        ui->status->setText("Disabled, binaries not found.");
    }

    ffmpegPath = QStandardPaths::findExecutable("ffmpeg");
    ffmpegPath = QStandardPaths::findExecutable("ffmpeg", {"/opt/homebrew/bin"});

    if (ffmpegPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "ffmpeg not found!");
        ui->downloadButton->setEnabled(false);
        ui->status->setText("Disabled, binaries not found.");
    }
}

MainGUI::~MainGUI() {
    if (process) {
        process->kill();
        delete process;
    }

    delete ui;
}

void MainGUI::on_directoryButton_clicked() {
    QString directoryPath = QFileDialog::getExistingDirectory(this, "Select destination directory", QDir::homePath());
    if (!directoryPath.isEmpty())
    {
        ui->pathPrint->setText(directoryPath);
    }
}

void MainGUI::on_downloadButton_clicked() {
    // reset labels
    ui->urlErrorLabel->hide();
    ui->pathErrorLabel->hide();

    QString url = ui->urlInput->text().trimmed();
    QString directoryPath = ui->pathPrint->text().trimmed();
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

    // connect to finished and newOutput events
    connect(process, &QProcess::finished, this, &MainGUI::onProcessFinished);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainGUI::onProcessNewOutput);

    process->setWorkingDirectory(directoryPath);

    // arguments for download
    QStringList args;
    args << "-x"
         << "--audio-format" << "mp3"
         << "--ffmpeg-location" << ffmpegPath
         << "-P" << directoryPath
         << url;

    ui->status->setStyleSheet("QLabel { color : gold; }");
    ui->status->setText("Starting download...");

    process->start(ytdlpPath, args);

    ui->status->setText("Downloading...");

    ui->downloadButton->setEnabled(false);
    ui->downloadButton->setText("Downloading...");
}

void MainGUI::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    ui->downloadButton->setEnabled(true);
    ui->downloadButton->setText("Download");

    // set status according to exit code
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        ui->status->setStyleSheet("QLabel { color : LawnGreen; }");
        ui->status->setText("Download finished!");
    } else {
        ui->status->setStyleSheet("QLabel { color : FireBrick; }");
        ui->status->setText("Error, download failed!");
    }
}

void MainGUI::updateProgressBar(int value) {
    ui->progressBar->setValue(value);
}

void MainGUI::onProcessNewOutput() {
    QString newOutputLine = process->readLine();

    // match download progress with regexp, update progress bar
    if (newOutputLine.contains("[download]")) {
        QRegularExpression regexp;
        regexp.setPattern(R"((\d+\.?\d*)%)");

        QRegularExpressionMatch match = regexp.match(newOutputLine);

        if (match.hasMatch()) {
            int capture = match.captured(1).toInt();
            updateProgressBar(capture);
        }
    }
}
