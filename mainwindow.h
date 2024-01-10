#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>
#include <QFileInfo>

void addDateTime(const QFileInfo &info);

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loopBtn_pressed();

    void on_concurrentBtn_pressed();

private:
    QFileInfoList getImagesInFolder();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
