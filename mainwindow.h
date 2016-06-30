#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtPrintSupport>
#include <QtNetwork/QtNetwork>

#include "downloaddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPrinter *printer;
    QString fileName;
    QString baseUrl;
    QNetworkAccessManager *netManager;
    DownloadDialog *dialog;
    QJsonArray fileUrls;
    QString savePath;
    void printFiles();

private slots:
    void on_print_clicked();
    void plotPic(QPrinter *);
};

#endif // MAINWINDOW_H
