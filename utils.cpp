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

void MainGUI::addArguments(const QString & url, const QString & directoryPath) {
    args // ----- GENERAL SETTINGS -----
        << "--newline"
        // << "-—embed-thumbnail";
        << "--add-metadata";

    args // ----- IP BLOCK AVOIDANCE -----
         << "--limit-rate" << RATE_LIMIT // limit download speed to not get blocked by YouTube
         << "--sleep-interval" << QString::number(SLEEP_MIN) // pause between videos when downloading playlist (minimum)
         << "--max-sleep-interval" << QString::number(SLEEP_MAX);  // pause between videos when downloading playlist (maximum)

    args // ----- PATHS -----
         << "--ffmpeg-location" << ffmpegPath
         << "-P" << directoryPath;

    QButtonGroup group;
    QList<QRadioButton *> allButtons = ui->formatTabs->findChildren<QRadioButton *>();

    for(int i = 0; i < allButtons.size(); i++)
    {
        group.addButton(allButtons[i],i);
    }

    QString name = group.checkedButton()->objectName();

    // ----- AUDIO or VIDEO -----
    if (ui->formatTabs->currentIndex() == 0) {
        // AUDIO is selected
        args << "-x";

        if (name == "mp3Button") {
            args << "--audio-format" << "mp3";
        } else if (name == "m4aButton") {
            args << "--audio-format" << "m4a";
        } else if (name == "flacButton") {
            args << "--audio-format" << "flac";
        } else if (name == "wavButton") {
            args << "--audio-format" << "wav";
        } else if (name == "opusButton") {
            args << "--audio-format" << "opus";
        } else if (name == "vorbisButton") {
            args << "--audio-format" << "vorbis";
        } else {
            args << "--audio-format" << "mp3";
        }

    } else {
        // VIDEO is selected
        if (name == "mp4Button") {
            args << "-S"
                 << "res,ext:mp4:m4a"
                 << "--recode"
                 << "mp4";

        } else if (name == "mkvButton") {
            args << "--merge-output-format"
                 << "mkv";

        } else if (name == "webmButton") {
            args << "-S"
                 << "res,ext:webm:opus"
                 << "--recode"
                 << "webm";

        } else if (name == "movButton") {
            args << "--remux-video" << "mov";

        } else if (name == "aviButton") {
            args << "--recode" << "avi";

        } else if (name == "flvButton") {
            args << "--recode" << "flv";

        } else {
            args << "--merge-output-format" << "mkv";
        }
    }

    args << url;
}
