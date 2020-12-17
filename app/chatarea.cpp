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
    QJsonObject emoticons = *emoticonData; //jsonDoc.object();

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
    char recvbuf[1] = {0};
    qint64 numBytesRead = socket->read(recvbuf, 1);
    qDebug() << "ChatArea: read " + QString::number(numBytesRead) + " bytes!";

    qint64 msgLengthInBytes = qint64(recvbuf[0]);

    qDebug() << "Message length to expect: " << QString::number(msgLengthInBytes) + " bytes.";
    char msgbuf[1024] = {0}; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something

    // read in message
    qint64 msgBytesRead = socket->read(msgbuf, msgLengthInBytes);

    QString msg = QString(msgbuf);
    qDebug() << "ChatArea: received message:" << msg;

    // check to see if we need to read more bytes
    if(msgBytesRead < msgLengthInBytes){
        qDebug() << "got an incomplete message from the server! need to read more bytes.";
        // TODO: read more bytes if needed
    }

    // check first char to determine what kind of message it is?
    QStringList tokens = msg.split(":");
    if(tokens.length() == 3){

        QString msgType = tokens[0];

        if(msgType.toInt() == int(MessageType::Message)){
            // regular message
            QString theMsg = tokens[2];

            QDateTime datetime;
            uint timestamp = uint(tokens[1].toInt()); // format is: msgType:timestamp:msg
            datetime.setTime_t(timestamp);
            QString now = datetime.toString(Qt::SystemLocaleShortDate);

            QString actualMsg = now + ":" + theMsg;

            ui->chatDisplay->append(actualMsg);

        }else if(msgType.toInt() == int(MessageType::CurrentUsers)){
            // in this case we're expecting a msg telling about a new client who joined
            // and a list of all currently connected users (in the same message)
            // the msg and the list of users will be separated by a semicolon for now

            qDebug() << "msg: " << tokens[2];
            QStringList msgTokens = tokens[2].split(";");

            QString newClientMsg = msgTokens[0];
            ui->chatDisplay->append(newClientMsg);

            ui->usersOnlineDisplay->clear();

            for(int i = 1; i < msgTokens.length(); i++){
                QString clientName = msgTokens[i];
                ui->usersOnlineDisplay->append(clientName.trimmed());
            }
        }
    }

}

void ChatArea::send(){
    QString msg = ui->enterMessage->text();
    msg = msg.trimmed();
    msg = "2:" + *(userData->username) + ":" + msg; // this is a bad format. what if the msg has colons??

    // prepend the message with the length (in bytes) of the whole message (including the type) so the server knows
    // exactly how many bytes to read for the complete message
    int msgByteLength = msg.toUtf8().size();
    msg = QString(msgByteLength) + msg;

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
}

void ChatArea::disconnect(){
    // disconnect from socket
    // return to login page (emit a signal for that?)
    // unhook signal/slots?
    // since we're disconnecting, delete username pointer in userdata
    delete userData->username;
    userData->username = nullptr;
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
