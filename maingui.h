#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainGUI;
}
QT_END_NAMESPACE

class MainGUI : public QMainWindow {
    Q_OBJECT

public:
    MainGUI(QWidget *parent = nullptr);
    ~MainGUI();

private slots:
    void on_directoryButton_clicked();

    void on_downloadButton_clicked();

    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void onProcessNewOutput();

    void updateProgressBar(int value);

private:
    Ui::MainGUI *ui;
    QProcess *process;
    QString ytdlpPath;
    QString ffmpegPath;

    void detectBinaries();
};
#endif // MAINGUI_H
