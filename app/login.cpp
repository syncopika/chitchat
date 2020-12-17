#include "login.h"
#include "userdata.h"
#include "ui_login.h"

Login::Login(QWidget *parent, QTcpSocket* socket) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(go()));
    this->socket = socket;
    this->userdata = nullptr;
    setUp();
}

void Login::setUp(){
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connectedToServer())); // why can't I do this in the constructor?
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
}

void Login::tearDown(){
    // disconnect signals/slots to not interfere with the chat page
    QObject::disconnect(socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));;
}

void Login::sendGreeting(){
    QString username = this->username;

    QString msgType;
    msgType.setNum(int(MessageType::Hello));

    QString usernameLength;
    usernameLength.setNum(username.length());

    QString greeting = msgType + ":" + usernameLength + ":"  + username;

    // send size of msg to expect first
    const char msgSize = (int)greeting.length();

    qint64 msgSizeBytesWritten = socket->write(&msgSize, 1);
    if(msgSizeBytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }

    std::string greet = greeting.toStdString();
    const char* gstring = greet.c_str();
    qDebug() << "going to send:" << gstring;

    qint64 bytesWritten = socket->write(gstring, greeting.length());
    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }
}

void Login::go()
{
    if(userdata == nullptr){
        // do this because if user disconnects, they'll come back to this page
        // and userdata should already exist
        userdata = new UserData();
    }

    username = ui->lineEdit->text();
    ipAddr   = ui->lineEdit_2->text();
    port     = ui->lineEdit_3->text();

    // TODO: validate username. make sure absence of certain characters like ; or : maybe?

    ui->progressBar->setValue(25);

    connectToServer(ipAddr, port.toUShort(), &(*socket));
}


void Login::connectedToServer()
{
    // send greeting
    qDebug() << "Login: connected to server. now sending a greeting";
    ui->progressBar->setValue(75);

    // collect avatar image + user info
    userdata->username = new QString(username);

    // emit signal to send its pointer to mainwindow
    emit sendUserData(userdata);

    sendGreeting();

    ui->progressBar->setValue(100);

    // disconnect signals for this page
    tearDown();

    // move to chat area screen
    emit goToChat();
}

void Login::disconnectedFromServer()
{
    qDebug() << "Login: disconnected from server.";
}

void Login::sendMessage()
{
    // message was written to socket
}

void Login::connectToServer(QString& ipAddr, quint16 port, QTcpSocket* socket)
{
    // attempt to connect to ip:port
    qDebug() << "Login: attempting to connect to: " << ipAddr << ":" << port;
    socket->connectToHost(ipAddr, port);

    if(socket->waitForConnected(3000)){
        qDebug() << "Login: successfully connected to server!";
        ui->progressBar->setValue(50);
    }else{
        qDebug() << "Login: failed to connect to server :(";
        ui->progressBar->setValue(0);
    }
}

Login::~Login()
{
    delete ui;
}
