#include "chatarea.h"
#include "userdata.h"
#include "ui_chatarea.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>

ChatArea::ChatArea(QWidget *parent, QTcpSocket* socket, QJsonObject* emoticonData) :
    QWidget(parent),
    ui(new Ui::ChatArea)
{
    ui->setupUi(this);
    this->socket = socket;
    this->emoticonData = emoticonData;

    setUp();
    setupEmoticons();
}

// have a signal from mainwindow to do setup? like when the page changes
void ChatArea::setUp(){
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(ui->sendMessage, SIGNAL(clicked()), this, SLOT(send()));
    QObject::connect(ui->comboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticons(const QString&)));
}

void ChatArea::tearDown(){
    QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    QObject::disconnect(ui->sendMessage, SIGNAL(clicked()), this, SLOT(send()));
    QObject::disconnect(ui->comboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticons(const QString&)));
}


void ChatArea::setupEmoticons(){
    QJsonObject emoticons = *emoticonData;

    // set up combo box with emoticons
    QComboBox* dropdown = ui->comboBox;
    foreach(const QString& key, emoticons.keys()){
        dropdown->addItem(key);
    }

    // show the current emoticons for the selected key
    QString selected = dropdown->currentText();
    updateEmoticons(selected);
}


void ChatArea::updateEmoticons(const QString& emoticonCategory){
    QJsonArray selectedEmoticons = emoticonData->value(emoticonCategory).toArray();

    ui->emoticonDisplay->clear();

    foreach(const QJsonValue& emote, selectedEmoticons){
        QString e = emote.toString();
        ui->emoticonDisplay->append(e);
    }
}

void ChatArea::receiveMessage(){
    // read 1 byte from socket first to know how many to read for the message
    // a message sent from the server should always be prefaced with the size of the message to expect
    char recvbuf[1] = {0};
    qint64 numBytesRead = socket->read(recvbuf, 1);
    qDebug() << "ChatArea: read " + QString::number(numBytesRead) + " bytes!";

    qint64 msgLengthInBytes = qint64(recvbuf[0]);

    qDebug() << "Message length to expect: " << QString::number(msgLengthInBytes) + " bytes.";
    char msgbuf[1024] = {0}; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something

    // read in message
    qint64 msgBytesRead = socket->read(msgbuf, msgLengthInBytes);

    qDebug() << "ChatArea: received message:" << QString(msgbuf);

    // check to see if we need to read more bytes
    if(msgBytesRead < msgLengthInBytes){
        qDebug() << "got an incomplete message from the server! need to read more bytes.";
        // TODO: read more bytes if needed
    }

    QByteArray msgBytes = QByteArray(msgbuf);
    QJsonDocument msgJson = QJsonDocument::fromJson(msgBytes);
    QJsonObject data = msgJson.object();
    QString msgType = data["MsgType"].toString();
    QString msg = data["Msg"].toString();
    QString sender = data["Sender"].toString();
    QString timestamp = data["Timestamp"].toString();

    if(msgType.toInt() == int(MessageType::Message)){
        // regular message
        QDateTime datetime;
        uint tstamp = uint(timestamp.toInt());
        datetime.setTime_t(tstamp);
        QString now = datetime.toString(Qt::SystemLocaleShortDate);

        QString actualMsg = sender + "@" + now + ": " + msg + "\n";

        ui->chatDisplay->append(actualMsg);

    }else if(msgType.toInt() == int(MessageType::CurrentUsers)){
        // in this case we're expecting a msg telling about a new client who joined
        // and a list of all currently connected users (in the same message)
        // the msg and the list of users will be separated by a semicolon for now
        qDebug() << "msg: " << msg;
        QStringList msgTokens = msg.split(";");

        QString newClientMsg = msgTokens[0];
        ui->chatDisplay->append(newClientMsg + "\n");

        ui->usersOnlineDisplay->clear();

        for(int i = 1; i < msgTokens.length(); i++){
            QString clientName = msgTokens[i];
            ui->usersOnlineDisplay->append(clientName.trimmed());
        }
    }
}

void ChatArea::send(){
    QString msg = ui->enterMessage->text().trimmed();

    if(msg != ""){
        bool msgSent = sendMessage(msg);
        if(!msgSent){
            qDebug() << "ChatArea: message failed to send! :(";
        }
    }

    // clear input
    ui->enterMessage->clear();
}

bool ChatArea::sendMessage(QString msg){
    QJsonObject regularMsg;
    regularMsg["Msg"] = msg;
    regularMsg["MsgType"] = "2"; // how can I get MessageType::Message to be a regular int? QString(MessageType::Message) results in "\u0002"
    regularMsg["Sender"] = *userData->username;
    regularMsg["Timestamp"] = ""; // clients don't need to send a timestamp

    QJsonDocument doc(regularMsg);
    QByteArray msgBytes = doc.toJson();

    // send size of msg to expect first
    const char msgSize = int(msgBytes.length());

    qint64 msgSizeBytesWritten = socket->write(&msgSize, 1);
    if(msgSizeBytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
    }

    qDebug() << "going to send:" << msgBytes;

    qint64 bytesWritten = socket->write(msgBytes, msgBytes.length());
    //socket->flush();

    if(bytesWritten == -1){
        qDebug() << "there was an error writing to the socket!";
        return false;
    }

    return true;
}

void ChatArea::getUserData(UserData* data){
    userData = data;
    ui->usernameLabel->setText(*(data->username));

    // show avatar
    if(data->avatar){
        QGraphicsView* view = ui->graphicsView;
        view->setScene(data->avatar);
        view->show();
        view->fitInView(data->avatar->sceneRect());
    }
}

void ChatArea::disconnect(){
    // mainwindow.cpp is handling socket disconnection currently (via menu bar).
    // I don't think there's any cleanup we need to do here?

    // disconnect from socket
    // return to login page (emit a signal for that?)
    // unhook signal/slots?
    // since we're disconnecting, delete username pointer in userdata
}

void ChatArea::addEmoticon(){
    //TODO: this should be taken care of in a separate widget/page
    // maybe make have a tab in a menubar to edit emoticons?
}

void ChatArea::removeEmoticon(){
    //TODO: this should be taken care of in a separate widget/page
}


ChatArea::~ChatArea()
{
    delete ui;
}
