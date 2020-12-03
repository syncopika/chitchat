#include "chatarea.h"
#include "userdata.h"
#include "ui_chatarea.h"

ChatArea::ChatArea(QWidget *parent, QTcpSocket* socket) :
    QWidget(parent),
    ui(new Ui::ChatArea)
{
    ui->setupUi(this);
    this->socket = socket;
    connect(ui->sendMessage, SIGNAL(clicked()), this, SLOT(send()));

    // also need to know who else is in the chat
    // also need username!
    // emit signal to mainwindow to receive userdata
}

void ChatArea::receiveMessage()
{
    // read from socket
    char recvbuf[1024]; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something
    socket->read(recvbuf, 1024);
    qDebug() << "received message: " << recvbuf;
}

void ChatArea::send(){
    QString msg = ui->enterMessage->text();
    msg = msg.trimmed();
    msg = "2:<some user>:" + msg; // TODO: need to know who sent the message! also a timestamp would be nice (oh but that contains colons :/ )"
    if(msg != ""){
        bool msgSent = sendMessage(msg);
        if(!msgSent){
            qDebug() << "message failed to send! :(";
        }
    }
}

bool ChatArea::sendMessage(QString msg){

    std::string message = msg.toStdString();
    const char* mstring = message.c_str();
    qDebug() << "going to send:" << mstring;

    qint64 bytesWritten = socket->write(mstring, message.length());
    socket->flush();

    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
        return false;
    }

    return true;
}

void ChatArea::getUserData(UserData* data){
    userData = data;
}

void ChatArea::disconnect(){
    // disconnect from socket
    // return to login page (emit a signal for that?)
}

void ChatArea::addEmoticon(){
    //TODO
}

void ChatArea::removeEmoticon(){
    //TODO
}


ChatArea::~ChatArea()
{
    delete ui;
}
