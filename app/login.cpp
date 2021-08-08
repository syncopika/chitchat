#include "login.h"
#include "userdata.h"
#include "ui_login.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

Login::Login(QWidget *parent, QTcpSocket* socket) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(go()));
    QObject::connect(ui->pushButton2, SIGNAL(clicked()), this, SLOT(importAvatarImage()));

    this->socket = socket;
    userdata = nullptr;
    scene = nullptr;
    avatar = nullptr;

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

void Login::resetProgressBar(){
    ui->progressBar->setValue(0);
}

void Login::sendGreeting(){
    QString username = this->username;

    QString msgType;
    msgType.setNum(int(MessageType::Hello));

    //QString usernameLength;
    //usernameLength.setNum(username.length());

    QJsonObject greetingMsg;
    greetingMsg["Msg"] = "hello from a new client";
    greetingMsg["MsgType"] = msgType;
    greetingMsg["Sender"] = username;
    greetingMsg["Timestamp"] = "";

    QJsonDocument doc(greetingMsg);
    QByteArray msgBytes = doc.toJson();

    // send size of msg to expect first
    const char msgSize = int(msgBytes.length());

    qint64 msgSizeBytesWritten = socket->write(&msgSize, 1);
    if(msgSizeBytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }

    qDebug() << "going to send:" << msgBytes;

    qint64 bytesWritten = socket->write(msgBytes, msgBytes.length());
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
    // these connections need to be reestablished when coming back to the page!
    tearDown();

    // move to chat area screen
    emit goToChat();

    // reset
    ui->progressBar->setValue(0);
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

// TODO: figure out how to actually use the image in chat
// maybe mainwindow should hold the image data and pass that around as needed to wdigets?
void Login::importAvatarImage(){
    QString filename = QFileDialog::getOpenFileName(this, "Open Image", QDir::currentPath());
    if(!filename.isEmpty()){
        QImage image(filename);

        if(image.isNull()){
            QMessageBox::information(this, "image view", "there was a problem loading the image");
            return;
        }

        QGraphicsView* view = ui->graphicsView;
        scene = new QGraphicsScene();
        view->setScene(scene);

        avatar = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        scene->addItem(avatar);
        view->show();
        view->fitInView(scene->sceneRect()); // still seems to leave some whitespace
    }
}

Login::~Login()
{
    // TODO: move this delete if moving these pointers to another widget
    //qDebug() << scene;
    //qDebug() << avatar;
    delete avatar;
    delete scene; // I think this might also delete this->avatar for us?
    //qDebug() << scene;
    //qDebug() << avatar;

    delete ui;
}
