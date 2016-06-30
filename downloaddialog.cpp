#include "downloaddialog.h"
#include<QtDebug>


DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent){
  stop_Btn=new QPushButton("stop");
  progressBar=new QProgressBar;
  progressBar->hide();

  layout=new QVBoxLayout();
  layout->addWidget(progressBar);
  layout->addWidget(stop_Btn);

  setLayout(layout);
  httpRequestAbort=false;
  connect(stop_Btn,SIGNAL(clicked()),this,SLOT(stop()));
}

DownloadDialog::~DownloadDialog(){
    this->stop();
}

void DownloadDialog::doDownloadFromUrls(){
    if(this->urls.size() > 0){
        this->current = this->current + 1;
        if(this->urls.size() > this->current){
            //开始下载
            this->httpDownload();
        }else{
            //文件下载完成，返回将文件返回到上一个界面
            this->accept();
        }
    }

}

void DownloadDialog::httpDownload(){

    //获得url并且发送请求
    QString currentUrl = this->urls[this->current].toString();
    int last = currentUrl.lastIndexOf ("/");
    QString fileName = currentUrl.right(currentUrl.length ()-last-1);

    QUrl url(currentUrl);

    file=new QFile(savePath + "/" + fileName);
    if (!file->open(QIODevice::WriteOnly)){
        delete file;
        file = 0;
        return;
    }

    accessManager=new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    reply=accessManager->get(request);//通过发送数据，返回值保存在reply指针里.
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finish为manager自带的信号，replyFinished是自定义的
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT( onDownloadProgress(qint64 ,qint64 )));//download文件时进度
    connect((QObject *)reply, SIGNAL(readyRead()),this, SLOT(onReadyRead()));
    progressBar->setValue(0);
    progressBar->show();

}


/***************响应结束**************************/
void DownloadDialog::replyFinished(QNetworkReply *reply){
  //获取响应的信息，状态码为200表示正常
  QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  QByteArray bytes = reply->readAll();  //获取字节
  QString result(bytes);  //转化为字符串
  qDebug()<<result;

  //无错误返回
  if(reply->error() == QNetworkReply::NoError)
  {
    QByteArray bytes = reply->readAll();  //获取字节
    QString result(bytes);  //转化为字符串
    qDebug()<<result;
    //下载完成，开始下一个文件的下载
    doDownloadFromUrls();
  }
  reply->deleteLater();//要删除reply，但是不能在repyfinished里直接delete，要调用deletelater;
}

/***********更新进度条*************/
void DownloadDialog::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal){
  progressBar->setMaximum(bytesTotal);
  progressBar->setValue(bytesSent);

}

void DownloadDialog::onReadyRead(){
    if(file)
        file->write(reply->readAll());
}

void DownloadDialog::stop(){
    if(!httpRequestAbort){
        reply->abort();//响应终止
        httpRequestAbort=true;
        stop_Btn->setText("start");

        //删除文件
        file->close();
        delete file;
        file = 0;
    }else{
        httpDownload();
        httpRequestAbort=false;
        stop_Btn->setText("stop");
    }
}
