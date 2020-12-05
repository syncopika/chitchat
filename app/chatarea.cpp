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
    setUp();
}

// have a signal from mainwindow to do setup? like when the page changes
void ChatArea::setUp(){
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
}

void ChatArea::tearDown(){
    QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

void ChatArea::receiveMessage(){
    // read from socket
    char recvbuf[1024] = {0}; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something
    socket->read(recvbuf, 1024);
    QString msg(recvbuf);
    msg = msg.trimmed();
    qDebug() << "ChatArea: received a message:" << msg;

    // check first char to determine what kind of message it is?
    QStringList tokens = msg.split(":");
    if(tokens.length() == 3){
        // put the msg in the UI
        ui->chatDisplay->append(tokens[1] + ":" + tokens[2]);
        socket->flush();
    }
}

void ChatArea::send(){
    QString msg = ui->enterMessage->text();
    msg = msg.trimmed();
    msg = "2:" + userData->username + ":" + msg; // this is a bad format. what if the msg has colons??

    if(msg != ""){
        bool msgSent = sendMessage(msg);
        if(!msgSent){
            qDebug() << "ChatArea: message failed to send! :(";
        }
    }
}

bool ChatArea::sendMessage(QString msg){
    std::string message = msg.toStdString();
    const char* mstring = message.c_str();
    qDebug() << "ChatArea: going to send:" << mstring;

    qint64 bytesWritten = socket->write(mstring, message.length());
    socket->flush();

    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
        return false;
    }

    return true;
}

void ChatArea::getUserData(UserData* data){
    qDebug() << "ChatArea: got the userdata!";
    userData = data;
}

void ChatArea::disconnect(){
    // disconnect from socket
    // return to login page (emit a signal for that?)
    // unhook signal/slots?
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
