#include "maingui.h"
#include "./ui_maingui.h"
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegularExpression>
#include <QStyleFactory>
#include <QColor>
#include <QPalette>
#include <QGuiApplication>
#include <QStyleHints>

MainGUI::MainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
    , process(nullptr) {

    ui->setupUi(this);

    // set error labels invisible by default
    ui->urlErrorLabel->hide();

    auto sp = ui->pathErrorLabel->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->pathErrorLabel->setSizePolicy(sp);
    ui->pathErrorLabel->hide();


    // connect color mode change in OS
    auto hints = QGuiApplication::styleHints();
    connect(hints, &QStyleHints::colorSchemeChanged, this, [this](Qt::ColorScheme scheme) {
        updateUIColors(scheme == Qt::ColorScheme::Dark);
    });

    // initial color update
    updateUIColors(hints->colorScheme() == Qt::ColorScheme::Dark);

    // progress bar not natively rendering in macOS (idk why), using Fusion style
    ui->progressBar->setStyle(QStyleFactory::create("Fusion"));

    detectBinaries();
}

void MainGUI::detectBinaries() {
    // find needed binaries, disable download if not found
    ytdlpPath = QStandardPaths::findExecutable("yt-dlp");
    if (ytdlpPath.isEmpty()) {
        ytdlpPath = QStandardPaths::findExecutable("yt-dlp", {"/opt/homebrew/bin"}); // for homebrew downloads
    }

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

    // reset progress bar
    ui->progressBar->setValue(0);

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
    args << "-x" // only audio
         << "--limit-rate" << "3M" // limit download speed to not get blocked by YouTube
         << "--sleep-interval" << "3" // pause between videos when downloading playlist (minimum)
         << "--max-sleep-interval" << "10"  // pause between videos when downloading playlist (maximum)
         << "--audio-format" << "mp3"
         << "--ffmpeg-location" << ffmpegPath
         << "-P" << directoryPath
         << url;

    setLabelColor(ui->status, downloadColor);
    ui->status->setText("Starting download...");

    process->start(ytdlpPath, args);

    ui->status->setText("Downloading...");

    ui->downloadButton->setEnabled(false);
    ui->downloadButton->setCursor(Qt::ArrowCursor);
    ui->downloadButton->setText("Downloading...");

    ui->directoryButton->setEnabled(false);
    ui->directoryButton->setCursor(Qt::ArrowCursor);
}

void MainGUI::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    ui->downloadButton->setEnabled(true);
    ui->downloadButton->setCursor(Qt::PointingHandCursor);
    ui->downloadButton->setText("Download");

    ui->directoryButton->setEnabled(true);
    ui->directoryButton->setCursor(Qt::PointingHandCursor);

    // set status according to exit code
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        setLabelColor(ui->status, finishedColor);
        ui->status->setText("Download finished!");
    } else {
        setLabelColor(ui->status, errorColor);
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
            double capture = match.captured(1).toDouble();
            updateProgressBar(static_cast<int>(capture));
        }
    }
}

// update label color
void MainGUI::setLabelColor(QLabel* label, QColor color) {
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, color);
    label->setPalette(palette);
}

void MainGUI::updateUIColors(bool isDark) {
    if (isDark) {
        errorColor = QColor(255, 100, 100);
        downloadColor = QColor(255, 214, 102);
        finishedColor = QColor(144, 238, 144);

        if (ui->status->palette().color(QPalette::WindowText) == QColor(200, 0, 0)) {
            setLabelColor(ui->status, errorColor);
        } else if (ui->status->palette().color(QPalette::WindowText) == QColor(184, 134, 11)) {
            setLabelColor(ui->status, downloadColor);
        } else if (ui->status->palette().color(QPalette::WindowText) == QColor(34, 139, 34)) {
            setLabelColor(ui->status, finishedColor);
        }

    } else {
        errorColor = QColor(200, 0, 0);
        downloadColor = QColor(184, 134, 11);
        finishedColor =  QColor(34, 139, 34);

        if (ui->status->palette().color(QPalette::WindowText) == QColor(255, 100, 100)) {
            setLabelColor(ui->status, errorColor);
        } else if (ui->status->palette().color(QPalette::WindowText) == QColor(255, 214, 102)) {
            setLabelColor(ui->status, downloadColor);
        } else if (ui->status->palette().color(QPalette::WindowText) == QColor(144, 238, 144)) {
            setLabelColor(ui->status, finishedColor);
        }
    }

    setLabelColor(ui->urlErrorLabel, errorColor);
    setLabelColor(ui->pathErrorLabel, errorColor);
}
