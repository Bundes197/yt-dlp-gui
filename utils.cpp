#include "maingui.h"
#include "./ui_maingui.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QButtonGroup>

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

QString MainGUI::getSelectedFormat() const {
    // Audio tab
    if (ui->formatTabs->currentIndex() == 0) {
        if (ui->mp3Button->isChecked()) return "mp3";
        if (ui->m4aButton->isChecked()) return "m4a";
        if (ui->flacButton->isChecked()) return "flac";
        if (ui->wavButton->isChecked()) return "wav";
        if (ui->opusButton->isChecked()) return "opus";
        if (ui->vorbisButton->isChecked()) return "vorbis";
        return "mp3";  // Default
    }
    // Video tab
    else {
        if (ui->mp4Button->isChecked()) return "mp4";
        if (ui->mkvButton->isChecked()) return "mkv";
        if (ui->webmButton->isChecked()) return "webm";
        if (ui->movButton->isChecked()) return "mov";
        if (ui->aviButton->isChecked()) return "avi";
        if (ui->flvButton->isChecked()) return "flv";
        return "mkv";  // Default
    }
}

void MainGUI::addArguments(const QString & url, const QString & directoryPath) {
    args // ----- GENERAL SETTINGS -----
        << "--newline"
        << "--add-metadata"
        << "--embed-thumbnail";
        // << "--embed-subs";

    args // ----- IP BLOCK AVOIDANCE -----
        << "--limit-rate" << RATE_LIMIT
        << "--sleep-interval" << QString::number(SLEEP_MIN)
        << "--max-sleep-interval" << QString::number(SLEEP_MAX);

    args // ----- PATHS -----
        << "--ffmpeg-location" << ffmpegPath
        << "-P" << directoryPath;

    // AUDIO and VIDEO
    QString format = getSelectedFormat();

    if (ui->formatTabs->currentIndex() == 0) {
        // AUDIO is selected
        args << "-x" << "--audio-format" << format;
    } else {
        // VIDEO is selected
        args << "-f" << "bestvideo+bestaudio/best";

        if (format == "mp4" || format == "mkv" || format == "webm") {
            args << "--merge-output-format" << format;
        } else {
            // MOV, AVI, FLV - recode needed
            args << "--recode-video" << format;
        }
    }

    if (ui->quantityTabs->currentIndex() == 0) {
        // SINGLE download is selected
    } else {
        // PLAYLIST download is selected
        if (ui->ignoreErrCheckBox->isChecked()) {
            // ignore error if one video from playlist cannot be downloaded
            args << "-i";
        }

        args << "--yes-playlist";
    }

    args << url;
}
