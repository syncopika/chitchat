#include "chatarea.h"
#include "ui_chatarea.h"

ChatArea::ChatArea(QWidget *parent, QTcpSocket* socket) :
    QWidget(parent),
    ui(new Ui::ChatArea)
{
    ui->setupUi(this);
    this->socket = socket;
}

void ChatArea::receiveMessage()
{
    // read from socket
    char recvbuf[1024]; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something
    //socket.read(recvbuf, 1024);
    //qDebug() << "received message: " << recvbuf;
}

ChatArea::~ChatArea()
{
    delete ui;
}
