#include "maingui.h"
#include "./ui_maingui.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QRegularExpression>
#include <QUrl>
#include <QDir>

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
        if (ui->bestButton->isChecked()) return "best";
        else if (ui->m4aButton->isChecked()) return "m4a";
        else if (ui->opusButton->isChecked()) return "opus";
        else if (ui->mp3Button->isChecked()) return "mp3";
        else if (ui->flacButton->isChecked()) return "flac";
        else if (ui->wavButton->isChecked()) return "wav";

        return "mp3";  // Default
    }
    // Video tab
    else {
        if (ui->mp4Button->isChecked()) return "mp4";
        else if (ui->mkvButton->isChecked()) return "mkv";
        else if (ui->webmButton->isChecked()) return "webm";
        else if (ui->movButton->isChecked()) return "mov";
        else if (ui->aviButton->isChecked()) return "avi";
        else if (ui->flvButton->isChecked()) return "flv";

        return "mkv";  // Default
    }
}

void MainGUI::addArguments(const QString & url, const QString & directoryPath) {
    args // ----- GENERAL SETTINGS -----
        << "--newline"
        << "--add-metadata"
        << "--embed-thumbnail";

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

    // SINGLE FILE AND PLAYLIST
    if (ui->quantityTabs->currentIndex() == 0) {
        // SINGLE download is selected
        QString fileName = ui->fileNameInput->text().trimmed();

        // disable path traversal and regex, protection against file rewrite
        QString cleanFileName = sanitizeFilename(fileName);

        if (!cleanFileName.isEmpty()) {
            args << "-o" << cleanFileName;
        }
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

QString MainGUI::sanitizeFilename(const QString & filename) {
    QString clean = filename.trimmed();

    clean.remove(QRegularExpression("[/\\\\]"));
    clean.remove("..");

    clean.remove(QRegularExpression("[<>:\"|?*]"));

    if (clean.length() > 255) {
        clean = clean.left(255);
    }

    return clean;
}

bool MainGUI::isValidUrl(const QString & url) {
    QUrl urlObj(url);

    if (!urlObj.isValid() || urlObj.scheme().isEmpty()) {
        return false;
    }

    QStringList allowedSchemes = {"http", "https"};
    return allowedSchemes.contains(urlObj.scheme().toLower());
}

bool MainGUI::isValidDirectory(const QString & path) {
    QDir dir(path);
    if (!dir.exists()) return false;

    QFileInfo dirInfo(path);
    return dirInfo.isWritable();
}

