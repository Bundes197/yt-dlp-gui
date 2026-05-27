#include "maingui.h"
#include "./ui_maingui.h"

void MainGUI::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
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