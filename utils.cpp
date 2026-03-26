#include "maingui.h"
#include "./ui_maingui.h"
#include <QStandardPaths>
#include <QMessageBox>

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
    if (ffmpegPath.isEmpty()) {
        ffmpegPath = QStandardPaths::findExecutable("ffmpeg", {"/opt/homebrew/bin"}); // for homebrew downloads
    }

    if (ffmpegPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "ffmpeg not found!");
        ui->downloadButton->setEnabled(false);
        ui->status->setText("Disabled, binaries not found.");
    }
}

void MainGUI::addArguments(const QString & url, const QString & directoryPath) {
    args // ----- IP BLOCK AVOIDANCE -----
         << "--limit-rate" << RATE_LIMIT // limit download speed to not get blocked by YouTube
         << "--sleep-interval" << QString::number(SLEEP_MIN) // pause between videos when downloading playlist (minimum)
         << "--max-sleep-interval" << QString::number(SLEEP_MAX);  // pause between videos when downloading playlist (maximum)

    args // ----- PATHS -----
         << "--ffmpeg-location" << ffmpegPath
         << "-P" << directoryPath;

    args // ----- AUDIO and VIDEO -----
         << "-x" // only audio, for testing
         << "--audio-format" << "mp3"; // for testing

    args << url;
}
