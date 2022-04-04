#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
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
};
#endif // MAINWINDOW_H
