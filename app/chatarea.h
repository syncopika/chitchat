#ifndef CHATAREA_H
#define CHATAREA_H

#include <QWidget>
#include <QTcpSocket>
#include "userdata.h"

namespace Ui {
    class ChatArea;
}

class ChatArea : public QWidget
{
    Q_OBJECT

public:
    explicit ChatArea(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~ChatArea();

    enum MessageType {
        Hello = 1,    //1
        Message,      //2
        Goodbye,      //3
        CurrentUsers, //4 - should deliver a list of current users
    };

private:
    Ui::ChatArea* ui;
    QTcpSocket* socket;
    QString messageInput;
    UserData* userData;

    void setUp();
    bool sendMessage(QString msg);

private slots:
    void receiveMessage();
    void send();
    void disconnect();
    void addEmoticon();
    void removeEmoticon();
    void getUserData(UserData* data);
};

#endif // CHATAREA_H
