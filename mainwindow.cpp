#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QtNetwork/QtNetwork>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->baseUrl = "http://127.0.0.1:8080/";
    this->netManager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_print_clicked(){
    //打印按钮按下
    QString userPk = ui->userid->text();
    if(!userPk.isEmpty()){
        //发送请求到后台取得
        QNetworkRequest req;
        QString requestUrl;
        requestUrl += this->baseUrl + "/user/getPrintUrls";
        req.setUrl(QUrl(requestUrl));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QByteArray post_data;
        post_data.append("{\"userid\":\"" + userPk + "\"}");
        QNetworkReply *reply = netManager->post(req, post_data);
        QEventLoop ev;
        connect(reply, SIGNAL(finished()), &ev, SLOT(quit()));
        ev.exec(QEventLoop::ExcludeUserInputEvents);
        if (reply->error() == QNetworkReply::NoError){
            QByteArray ba = reply->readAll();
            QJsonParseError jsonpe;
            QJsonDocument json = QJsonDocument::fromJson(ba, &jsonpe);
            if (jsonpe.error == QJsonParseError::NoError){
                if (json.isObject())
                {
                    QJsonObject obj = json.object();
                    if (obj.contains("code")){
                        QString code = obj["code"].toString();
                        if(code == "0"){
                            if(obj["information"].isArray()){
                                fileUrls = obj["information"].toArray();

                                //打开下载窗口开始下载
                                savePath = QFileDialog::getExistingDirectory(this, "Open Directory", ".", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                                dialog = new DownloadDialog;
                                dialog->urls = fileUrls;
                                dialog->savePath = savePath;
                                dialog->doDownloadFromUrls();
                                int result = dialog->exec();
                                if(result ==  QDialog::Accepted){
                                    //开始打印了
                                    this->printFiles();
                                }
                            }

                        }else{

                        }

                        QJsonArray jarray = obj["fileList"].toArray();
                        for (int row = 0; row < jarray.size(); row++)
                        {
                            qDebug() << "====Row[" << row << "] Start===================";
                            QJsonObject jobj = jarray[row].toObject();
                            for (QJsonObject::Iterator it = jobj.begin();
                                 it != jobj.end(); it++)
                            {
                                qDebug() << it.key() << " => " << it.value();
                            }
                        }
                    }
                }
            }
        }
    }


}

void MainWindow::printFiles(){
    QPrinter printer(QPrinter::HighResolution);

    QPrintPreviewDialog preview(&printer,0);    /* 打印预览 */

    /**
     * QPrintPreviewDialog类提供了一个打印预览对话框，里面功能比较全，
     * paintRequested(QPrinter *printer)是系统提供的，
     * 当preview.exec()执行时该信号被触发，
     * plotPic(QPrinter *printer)是用户自定义的槽函数，图像的绘制就在这个函数里。
     */
    connect(&preview, SIGNAL(paintRequested(QPrinter *)),this,SLOT(plotPic(QPrinter *)));

    preview.exec(); /* 等待预览界面退出 */

}

void MainWindow::plotPic(QPrinter *printer)
{
    QPainter painter;
    painter.begin(printer);

    //遍历打印
    //获得url并且发送请求
    QJsonArray urls = this->fileUrls;
    for(int i=0;i<urls.size();i++){
        QString currentUrl = urls[i].toString();
        int last = currentUrl.lastIndexOf ("/");
        QString fileName = currentUrl.right(currentUrl.length ()-last-1);

        QImage img(savePath + "/" + fileName);

        QRect rect = painter.viewport();
        QSize size = img.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);     //此处保证图片显示完整
        painter.setViewport(rect.x(), rect.y(),size.width(), size.height());
        painter.setWindow(img.rect());
        painter.drawImage(0,0,img);

        if(i != urls.size() -1)
            printer->newPage();
    }

    painter.end();

//    painter.drawPixmap(0,0,img); /* 数据显示至预览界面 */
}

