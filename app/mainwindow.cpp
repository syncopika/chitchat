#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QHostAddress>

// this looks helpful! and interesting to study:
// https://code.qt.io/cgit/qt/qtbase.git/tree/examples/network/network-chat?h=5.15
// https://forum.qt.io/topic/100044/qtcpsocket-read-large-data-from-server/2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // when go button is pressed,
    // read in username and ip:port to connect to
    // and attempt to connect
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

    QTcpSocket socket;
    bool connected = connectToServer(username, ipAddr, port.toUShort(), &socket);
}

bool MainWindow::connectToServer(QString& username, QString& ipAddr, quint16 port, QTcpSocket* socket)
{
    // attempt to connect to ip:port
    qDebug() << "attempting to connect to: " << ipAddr << ":" << port;
    socket->connectToHost(ipAddr, port);

    if(socket->waitForConnected(3000)){
        qDebug() << "successfully connected to server!";
    }else{
        qDebug() << "failed to connect to server :(";
        return false;
    }

    // send greeting
    // also neat: https://www.qtcentre.org/threads/33924-QString-use-c_str()
    QString greeting = "hello:" + username;
    std::string greet = greeting.toStdString();
    const char* gstring = greet.c_str();
    qDebug() << gstring;
    socket->write(gstring, greeting.length() + 1); // +1 for null-term

    // read ok msg from socket

    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}