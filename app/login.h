#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
    class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~Login();

    enum MessageType {
        Hello = 1,   //1
        Message,     //2
        Goodbye      //3
    };

signals:
    void goToChat();

public slots:

private slots:
    void go();
    void connected();
    void disconnected();
    void sendMessage();
    void receiveMessage();

private:
    Ui::Login* ui;

    QTcpSocket* socket;
    QString username;
    QString ipAddr;
    QString port;

    void connectToServer(QString& ipAddr, quint16 port, QTcpSocket* socket);

};

#endif // LOGIN_H
