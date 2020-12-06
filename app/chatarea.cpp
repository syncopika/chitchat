#include "chatarea.h"
#include "userdata.h"
#include "ui_chatarea.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

ChatArea::ChatArea(QWidget *parent, QTcpSocket* socket) :
    QWidget(parent),
    ui(new Ui::ChatArea)
{
    ui->setupUi(this);
    this->socket = socket;

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
    // load in emoticons
    // TODO: check for a local json as well?
    // TODO: and also allow editing/addition/removal of emoticons
    // on a separate page/widget?
    QString defaultEmoticons(
       "{"
       "  \"happy\": ["
       "      \":D\", "
       "      \":)\", "
       "      \"^_^\" "
       "   ],"
       "  \"sad\": ["
       "      \":(\", "
       "      \":<\" "
       "   ],"
       "  \"angry\": ["
       "      \">:|\", "
       "      \"（　ﾟДﾟ）\", "
       "      \"(╯°□°)╯︵ ┻━┻\" "
       "   ],"
       "  \"funny\": ["
       "      \"¯\\_(ツ)_/¯\", "
       "      \"ʕ•ᴥ•ʔ\" "
       "   ]"
       "}"
    );
    QByteArray emoticonData = defaultEmoticons.toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(emoticonData));
    emoticons = jsonDoc.object();

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
    QJsonArray selectedEmoticons = emoticons.value(emoticonCategory).toArray();

    ui->emoticonDisplay->clear();

    foreach(const QJsonValue& emote, selectedEmoticons){
        QString e = emote.toString();
        ui->emoticonDisplay->append(e);
    }
}

void ChatArea::receiveMessage(){
    // read from socket
    char recvbuf[1024] = {0}; // TODO: make 1024 a typedef? like DEFAULT_BUF_LEN or something
    qint64 numBytesRead = socket->read(recvbuf, 1024);
    qDebug() << "ChatArea: read " + QString::number(numBytesRead) + " bytes!";
    QString msg(recvbuf);
    msg = msg.trimmed();
    qDebug() << "ChatArea: received a message:" << msg;

    // check first byte to know the length of the message and see if we need more bytes
    if((int)recvbuf[0] != msg.length() - 1){
        qDebug() << "got an incomplete message from the server! need to read more bytes.";
        // TODO: read more bytes if needed
    }

    // remove the first char from the message since that's just supposed to represent
    // the length of the message
    msg = msg.remove(0, 1);

    // check first char to determine what kind of message it is?
    QStringList tokens = msg.split(":");
    if(tokens.length() == 3){
        // put the msg in the UI
        ui->chatDisplay->append(tokens[2]);
    }
}

void ChatArea::send(){
    QString msg = ui->enterMessage->text();
    msg = msg.trimmed();
    msg = "2:" + *(userData->username) + ":" + msg; // this is a bad format. what if the msg has colons??

    // prepend the message with the length of the whole message (including the type) so the server knows
    // exactly how many bytes to read for the complete message
    msg = QString(msg.length()) + msg;

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
