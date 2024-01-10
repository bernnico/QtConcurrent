#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDial>
#include <QFileDialog>
#include <QDateTime>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>

#include <opencv2/opencv.hpp>

void addDateTime(const QFileInfo &info)
{
    cv::Mat image = cv::imread(info.absoluteFilePath().toStdString());
    if (!image.empty()) {
        QString dateTime = info.birthTime().toString();
        cv::putText(image,
                    dateTime.toStdString(),
                    cv::Point(30,30), // 25 pixels offset from th corner
                    cv::FONT_HERSHEY_PLAIN,
                    1.0,
                    cv::Scalar(0,0,255)); // red
        cv::imwrite(info.absoluteFilePath().toStdString(), image);
    }
}

bool filterImage(const QFileInfo &info)
{
    int minute = info.birthTime().time().minute(); // -1 if the time is invalid.
    int second = info.birthTime().time().second(); // -1 if the time is invalid.

    return  minute != -1 && second != -1
           && minute == 29 && second == 24;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QFileInfoList MainWindow::getImagesInFolder()
{
    QDir dir(QFileDialog::getExistingDirectory(this, tr("Open Images Folder")));

    return dir.entryInfoList(QStringList()
                             << "*.jpg"
                             << "*.png",
                             QDir::NoDotAndDotDot | QDir::Files,
                             QDir::Name);
}

void MainWindow::on_loopBtn_pressed()
{
    QFileInfoList list = getImagesInFolder();

    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    ui->progressBar->setRange(0, list.count()-1);

    for (int i = 0; i < list.count(); i++) {
        addDateTime(list[i]);
        ui->progressBar->setValue(i);

        // You can call this function occasionally
        // when your program is busy performing a long operation
        // (e.g. copying a file).
        qApp->processEvents();
    }
    qint64 e = elapsedTimer.elapsed();

    QMessageBox::information(this,
                             tr("Done!"),
                             QString(tr("Process %1 images in %2 milliseconds"))
                                 .arg(list.count())
                                 .arg(e));
}

void MainWindow::on_concurrentBtn_pressed()
{
    QFileInfoList list = getImagesInFolder();

    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    // optional
    // QThreadPool::globalInstance()->setMaxThreadCount(16);

    // QtConcurrent::blockingFilter(list, filterImage);

    QFuture<void> future = QtConcurrent::map(list, addDateTime);

    // QFutureWatcher must remain alive for the entire time the process continues.
    // The pointer will be deleted later when the process is complete.
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);

    // IMPORTANT! First, make all required connections for QFutureWatcher.
    connect(watcher,
            SIGNAL(progressRangeChanged(int,int)),
            ui->progressBar,
            SLOT(setRange(int,int)));

    connect(watcher,
            SIGNAL(progressValueChanged(int)),
            ui->progressBar,
            SLOT(setValue(int)));

    connect(watcher,
            &QFutureWatcher<void>::finished, // not SIGNAL(finished())
            [=]()
            {
                qint64 e = elapsedTimer.elapsed();
                QMessageBox::information(this,
                                         tr("Done!"),
                                         QString(tr("Processed %1 images in %2 milliseconds"))
                                             .arg(list.count())
                                             .arg(e));
            });

    connect(watcher,
            SIGNAL(finished()),
            watcher,
            SLOT(deleteLater()));

    // IMPORTANT! Finilly, set the future after all the connections are made.
    watcher->setFuture(future);
}





























