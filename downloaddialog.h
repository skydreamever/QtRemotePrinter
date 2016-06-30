#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QFile>
#include <QtNetwork/QtNetwork>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIODevice>
#include <QFileInfo>
#include <QProgressBar>

class DownloadDialog : public QDialog{
     Q_OBJECT
public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();
    QJsonArray urls;
    QString savePath;
    void doDownloadFromUrls();
protected slots:
  void httpDownload();
  void replyFinished(QNetworkReply*reply);
  void onDownloadProgress(qint64 bytesSent,qint64 bytesTotal);
  void onReadyRead();//
  void stop();//暂停，重新下载
private:
  QPushButton *stop_Btn;
  QProgressBar *progressBar;
  QFile *file;
  QFileInfo fileInfo;
  QNetworkAccessManager *accessManager;
  QNetworkReply *reply;
  QVBoxLayout *layout;
  bool httpRequestAbort; //请求停止的标志位,false表示允许请求
  int current = -1;
};

#endif // DOWNLOADDIALOG_H
