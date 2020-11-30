#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(go()));
}

void Login::go()
{
    // how to get the username available in the chat page?
    QString username = ui->lineEdit->text();
    QString ipAddr   = ui->lineEdit_2->text();
    QString port     = ui->lineEdit_3->text();

    qDebug() << "the username is: " << username;
    qDebug() << "the ip addr is: " << ipAddr;
    qDebug() << "the port is: " << port;
    qDebug() << "-------------";
    ui->progressBar->setValue(25);

    connect(&socket, SIGNAL(connected()), this, SLOT(connected())); // why can't I do this in the constructor?
    connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&socket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendMessage())); // this one might not be necessary?
    connect(&socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

    connectToServer(ipAddr, port.toUShort(), &socket);
}


void Login::connected()
{
    // send greeting
    qDebug() << "connected to server. now sending a greeting";
    ui->progressBar->setValue(75);

    QString msgType;
    msgType.setNum((int)MessageType::Hello);

    QString usernameLength;
    usernameLength.setNum(username.length());

    QString greeting = msgType + ":" + usernameLength + ":"  + username;

    std::string greet = greeting.toStdString();
    const char* gstring = greet.c_str();
    qDebug() << "going to send:" << gstring;

    qint64 bytesWritten = socket.write(gstring, greeting.length());
    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }
    this->socket.flush();

    ui->progressBar->setValue(100);
    // move to chat area screen
    emit goToChat();
}

void Login::disconnected()
{
    qDebug() << "disconnected from server.";
}

void Login::sendMessage()
{
    // message was written to socket
}

void Login::receiveMessage()
{
    // read from socket
    char recvbuf[1024]; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something
    socket.read(recvbuf, 1024);
    qDebug() << "received message: " << recvbuf;
}

void Login::connectToServer(QString& ipAddr, quint16 port, QTcpSocket* socket)
{
    // attempt to connect to ip:port
    qDebug() << "attempting to connect to: " << ipAddr << ":" << port;
    socket->connectToHost(ipAddr, port);

    if(socket->waitForConnected(3000)){
        qDebug() << "successfully connected to server!";
        ui->progressBar->setValue(50);
    }else{
        qDebug() << "failed to connect to server :(";
    }
}

Login::~Login()
{
    delete ui;
}
