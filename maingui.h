#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include <QProcess>
#include <QtWidgets/qlabel.h>

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
    QStringList args;

    static constexpr const char* RATE_LIMIT = "3M";
    static constexpr int SLEEP_MIN = 3;
    static constexpr int SLEEP_MAX = 10;

    QColor errorColor;
    QColor downloadColor;
    QColor finishedColor;

    static const QRegularExpression progressRegex;

    void detectBinaries();

    QString getSelectedFormat() const;

    void addArguments(const QString & url, const QString & directoryPath);

    void setLabelColor(QLabel * label, QColor color);

    void updateUIColors(bool isDark);

    void setButtonsEnabled(bool enabled);
};
#endif // MAINGUI_H
