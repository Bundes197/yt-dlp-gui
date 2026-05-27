#include "maingui.h"
#include "./ui_maingui.h"

void MainGUI::changeEvent(QEvent * event)
{
    if (ui && ui->status) {
        QColor currentColor = ui->status->palette().color(QPalette::WindowText);

        ui->retranslateUi(this);

        if (currentColor == finishedColor) {
            ui->status->setText(tr("Download finished!"));
        }
        else if (currentColor == errorColor) {
            ui->status->setText(tr("Error, download failed!"));
        }
        else if (process && process->state() == QProcess::Running) {
            ui->status->setText(tr("Downloading..."));
        }
    }

    QMainWindow::changeEvent(event);
}

void MainGUI::on_enButton_clicked() {
    qApp->removeTranslator(&appTranslator);

    QEvent languageChangeEvent(QEvent::LanguageChange);
    QGuiApplication::sendEvent(this, &languageChangeEvent);
}

void MainGUI::on_czButton_clicked() {
    qApp->removeTranslator(&appTranslator);

    if (appTranslator.load(":/i18n/yt-dlp-gui_cs")) {
        qApp->installTranslator(&appTranslator);
    }
}