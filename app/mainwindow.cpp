#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QHostAddress>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTcpSocket socket;

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(go()));
}

void MainWindow::go()
{
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
    connect(&socket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendMessage()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

    connectToServer(username, ipAddr, port.toUShort(), &socket);
}

void MainWindow::connected()
{
    // send greeting
    qDebug() << "connected to server. now sending a greeting";

    QString greeting = "1"; //MessageType::Hello + username.length() + username;
    std::string greet = greeting.toStdString();
    const char* gstring = greet.c_str();
    qDebug() << "going to send:" << gstring;

    qint64 bytesWritten = socket.write(gstring, greeting.length());
    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }
    this->socket.flush();
}

void MainWindow::disconnected()
{
    qDebug() << "disconnected from server.";
}

void MainWindow::sendMessage()
{
    // write to socket
}

void MainWindow::receiveMessage()
{
    // read from socket
    char recvbuf[1024]; // TODO: make a typedef? like DEFAULT_BUF_LEN or something
    socket.read(recvbuf, 1024);
    qDebug() << "received message: " << recvbuf;
}

void MainWindow::connectToServer(QString& username, QString& ipAddr, quint16 port, QTcpSocket* socket)
{
    // attempt to connect to ip:port
    qDebug() << "attempting to connect to: " << ipAddr << ":" << port;
    socket->connectToHost(ipAddr, port);

    if(socket->waitForConnected(3000)){
        qDebug() << "successfully connected to server!";
    }else{
        qDebug() << "failed to connect to server :(";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
