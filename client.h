#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork/QTcpSocket>
#include <QImage>
#include <QDialog>
QT_BEGIN_NAMESPACE
namespace Ui {class Client;}
QT_END_NAMESPACE
class Client : public QDialog
{
private:
    Ui::Client *ui;
    QTcpSocket *tcpClient;
    qint64 totalBytes;
    qint64 payloadSize;
    QString filename;
    QByteArray block;
    QImage image;
    bool isOK;
private slots:
    void openFile();
    void send();
    void connectServer();
    void startTransfer();
    void tcpConnected();
    void tcpDisconnected();
    QByteArray getImageData(const QImage&);
    void on_openButton_clicked();// Открываем картинку
    void on_sendButton_clicked();// отправляем картинки
    void on_connectButton_clicked();// Подключаем или отключаем сервер
signals:
    void buildConnected();// После подключения к серверу сигнал отправлен
public:
    Client(QWidget*);
};

#endif // CLIENT_H
