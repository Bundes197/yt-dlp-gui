#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainGUI;
}
QT_END_NAMESPACE

class MainGUI : public QMainWindow
{
    Q_OBJECT

public:
    MainGUI(QWidget *parent = nullptr);
    ~MainGUI();

private slots:
    void on_directoryButton_clicked();

    void on_downloadButton_clicked();

private:
    Ui::MainGUI *ui;
};
#endif // MAINGUI_H
