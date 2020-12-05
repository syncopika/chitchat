#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>
#include "userdata.h"

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
        Hello = 1,    //1
        Message,      //2
        Goodbye,      //3
        CurrentUsers, //4 - should deliver a list of current users
    };

signals:
    void goToChat();
    void sendUserData(UserData* data);

public slots:

private slots:
    void go();
    void connectedToServer();
    void disconnectedFromServer();
    void sendMessage();

private:
    Ui::Login* ui;

    QTcpSocket* socket;
    QString username;
    QString ipAddr;
    QString port;
    UserData* userdata;

    void setUp();
    void tearDown();
    void sendGreeting();
    void connectToServer(QString& ipAddr, quint16 port, QTcpSocket* socket);
};

#endif // LOGIN_H
