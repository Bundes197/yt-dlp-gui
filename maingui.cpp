#include "maingui.h"
#include "./ui_maingui.h"
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QProgressBar>
#include <QRegularExpression>
#include <QStyleFactory>
#include <QColor>
#include <QPalette>
#include <QGuiApplication>
#include <QStyleHints>
#include <QMessageBox>
#include <QUrl>
#include <QFile>

MainGUI::MainGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGUI)
    , process(nullptr) {

    ui->setupUi(this);

    // set error label invisible by default and remain size
    auto sp = ui->errorLabel->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->errorLabel->setSizePolicy(sp);
    ui->errorLabel->hide();


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

MainGUI::~MainGUI() {
    if (process) {
        process->kill();
        delete process;
    }

    delete ui;
}

void MainGUI::on_directoryButton_clicked() {
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Select destination directory"), QDir::homePath());
    if (!directoryPath.isEmpty())
    {
        ui->pathPrint->setText(directoryPath);
    }
}

void MainGUI::on_downloadButton_clicked() {
    if (!QFile::exists(ytdlpPath) || !QFile::exists(ffmpegPath)) {
        QMessageBox::warning(this, tr("Warning"), tr("Binaries disappeared! Checking again..."));

        detectBinaries();
        return;
    }

    // reset label
    ui->errorLabel->hide();

    // reset progress bar
    ui->progressBar->setValue(0);

    QString url = ui->urlInput->text().trimmed();
    if (url.isEmpty()) {
        ui->errorLabel->setText(tr("URL is missing!"));
        ui->errorLabel->show();
        return;
    }

    if (!isValidUrl(url)) {
        ui->errorLabel->setText(tr("Invalid URL!"));
        ui->errorLabel->show();
        return;
    }

    QString directoryPath = ui->pathPrint->text().trimmed();
    if (directoryPath.isEmpty()) {
        ui->errorLabel->setText(tr("Directory path is missing!"));
        ui->errorLabel->show();
        return;
    }

    if (!isValidDirectory(directoryPath)) {
        ui->errorLabel->setText(tr("Cannot write into selected directory!"));
        ui->errorLabel->show();
        return;
    }

    ui->errorLabel->hide();

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
    args.clear();
    addArguments(url, directoryPath);

    setLabelColor(ui->status, downloadColor);
    ui->status->setText(tr("Starting download..."));

    process->start(ytdlpPath, args);

    if (!process->waitForStarted(3000)) {
        setLabelColor(ui->status, errorColor);

        ui->status->setText(tr("Failed to start download!"));
        ui->downloadButton->setEnabled(true);
        ui->downloadButton->setCursor(Qt::PointingHandCursor);

        ui->downloadButton->setText(tr("Download"));
        ui->directoryButton->setEnabled(true);
        ui->directoryButton->setCursor(Qt::PointingHandCursor);
        return;
    }

    ui->status->setText(tr("Downloading..."));
    setButtonsEnabled(false);
    ui->downloadButton->setText(tr("Downloading..."));
}

void MainGUI::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    setButtonsEnabled(true);
    ui->downloadButton->setText(tr("Download"));

    // set status according to exit code
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        setLabelColor(ui->status, finishedColor);
        ui->status->setText(tr("Download finished!"));
    } else {
        setLabelColor(ui->status, errorColor);
        ui->status->setText(tr("Error, download failed!"));

        QString errorOutput = process->readAllStandardError();
        if (errorOutput.isEmpty()) {
            errorOutput = tr("Unknown error (exit code: %1)").arg(exitCode);
        }

        QMessageBox::critical(this, tr("Download Failed"),
                              tr("The download process failed.\n\nError details:\n%1").arg(errorOutput));
    }
}

const QRegularExpression MainGUI::progressRegex(R"((\d+\.?\d*)%)");

void MainGUI::onProcessNewOutput() {
    while (process->canReadLine()) {
        QString line = process->readLine().trimmed();

        // match download progress with regexp, update progress bar
        if (line.contains("[download]")) {
            QRegularExpressionMatch match = progressRegex.match(line);

            if (match.hasMatch()) {
                double percentage = match.captured(1).toDouble();
                int value = qRound(percentage);
                ui->progressBar->setValue(value);
            }
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

    setLabelColor(ui->errorLabel, errorColor);
}

void MainGUI::setButtonsEnabled(bool enabled) {
    ui->downloadButton->setEnabled(enabled);
    ui->downloadButton->setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
    ui->directoryButton->setEnabled(enabled);
    ui->directoryButton->setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
}
