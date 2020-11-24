#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum MessageType {
        Hello = 1,   //1
        Message,     //2
        Goodbye      //3
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void go();
    void connected();
    void disconnected();
    void sendMessage();
    void receiveMessage();

private:
    Ui::MainWindow *ui;
    QTcpSocket socket;
    QString username;
    QString ipAddr;
    QString port;

    void connectToServer(QString& ipAddr, quint16 port, QTcpSocket* socket);
};

#endif // MAINWINDOW_H
