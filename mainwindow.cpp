#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCompleter>
#include <QBuffer>
#include "QFileDialog"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList hostWordList, portWordList;
    hostWordList <<tr("127.0.0.1");
    portWordList << tr("6666");
    QCompleter* completerHost = new QCompleter(hostWordList, this);
    QCompleter* completerPort = new QCompleter(portWordList, this);
    ui->hostLineEdit->setCompleter(completerHost);
    ui->portLineEdit->setCompleter(completerPort);
    ui->hostLineEdit->setPlaceholderText(tr("127.0.0.1"));
    ui->portLineEdit->setPlaceholderText(tr("6666"));
    payloadSize = 64 * 1024; // 64KB
    totalBytes = 0;
    isOK = false;
    ui->sendButton->setEnabled(false);

    tcpClient = new QTcpSocket(this);

    // Когда соединение с сервером успешно, отправляется сигнал connected () и isOK устанавливается в true
    connect(tcpClient, SIGNAL(connected()), this, SLOT(tcpConnected()));
    // Когда нажимается кнопка отправки (и isOK истинно), выдается сигнал buildConnected () для начала передачи данных
    connect(this, SIGNAL(buildConnected()), this, SLOT(startTransfer()));
    // При отключении отправляем disconnected (), для isOK установлено значение false
    connect(tcpClient, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openButton_clicked()
{
    ui->clientStatus->setText(tr("Статус: Ожидание открытия файла!"));
    openFile();

}


void MainWindow::openFile()
{
    filename = QFileDialog::getOpenFileName(this);

    if (!filename.isEmpty()) {

        // Получаем фактическое имя файла
       auto currentImageName = filename.right(filename.size()
                                                 - filename.lastIndexOf('/')-1);

        ui->clientStatus->setText(tr("Файл успешно открыт!").arg(currentImageName));

        if(isOK == true){
            ui->sendButton->setEnabled(true);
        }
    }
}



void MainWindow::send()
{
    if(!isOK){
        ui->clientStatus->setText(tr("Пожалуйста, сначала подключитесь к серверу"));
        return;
    }else{
        // передаем сигнал
        emit buildConnected();
    }
}



void MainWindow::connectServer()
{
    ui->clientStatus->setText(tr("Подключение…"));

// // Подключение к серверу
    tcpClient->connectToHost(ui->hostLineEdit->text(),
                             ui->portLineEdit->text().toInt());

    isOK = true;
}


void MainWindow::startTransfer()
{
    QDataStream sendOut(&block, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_6);

   // Получаем данные изображения
    QImage image(filename);
    QString imageData = getImageData(image);


    // Зарезервируйте информационное пространство общего размера, информационное пространство размера изображения, а затем введите информацию об изображении
    sendOut << qint64(0) << qint64(0) << imageData;

    // Общий размер здесь - это сумма информации об общем размере, информации о размере изображения и фактической информации об изображении
    totalBytes += block.size();
    sendOut.device()->seek(0);

    // Возвращаемся в начало outBolock, заменяем два пробела qint64 (0) информацией о фактическом размере
    sendOut << totalBytes << qint64((block.size() - sizeof(qint64)*2));

    // Отправляем сигнал readyRead ()
    tcpClient->write(block);


    block.resize(0);

    ui->clientStatus->setText(tr("Передача файла произошла успешно"));
    totalBytes = 0;
}


void MainWindow::tcpConnected()
{
    isOK = true;
    ui->connectButton->setText(tr("Отключить"));

    ui->clientStatus->setText(tr("Связаны"));
}

void MainWindow::tcpDisconnected()
{
    isOK = false;
    tcpClient->abort();
    ui->connectButton->setText(tr("Связь"));

    ui->clientStatus->setText(tr("Линия отключена"));
}

QByteArray MainWindow::getImageData(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "png");
    imageData = imageData.toBase64();

    return imageData;
}



void MainWindow::on_sendButton_clicked()
{
    send();
}

void MainWindow::on_connectButton_clicked()
{
    if (ui->connectButton->text() == tr("Связь")) {
            tcpClient->abort();
            connectServer();
        } else {
            tcpClient->abort();
        }
}
