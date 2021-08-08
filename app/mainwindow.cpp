#include "mainwindow.h"
#include "userdata.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QMenuBar>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isConnected = false;
    userData = nullptr;
    clientSocket = nullptr;
    emoticonData = nullptr;

    // set up qwidgets for the windows
    stackedWidget = new QStackedWidget;
    QMainWindow::setCentralWidget(stackedWidget);

    // set up menu bar
    setUpMenuBar();

    // set up emoticon stuff
    setUpEmoticons();

    clientSocket = new QTcpSocket();

    loginPage = new Login(nullptr, clientSocket);
    chatArea = new ChatArea(nullptr, clientSocket, emoticonData);
    emoticonEdit = new EmoticonEdit(nullptr, emoticonData);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(chatArea);
    stackedWidget->addWidget(emoticonEdit);
    stackedWidget->setCurrentWidget(loginPage);

    // receive signal from the login page to move to chat area
    connect(loginPage, SIGNAL(sendUserData(UserData*)), this, SLOT(getUserData(UserData*)));
    connect(loginPage, SIGNAL(goToChat()), this, SLOT(goToChat()));
    connect(this, SIGNAL(giveUserData(UserData*)), chatArea, SLOT(getUserData(UserData*)));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(resize(int))); // currentChanged is a built-in signal
}

void MainWindow::goToLogin(){
    isConnected = false;
    disconnectAct->setEnabled(false);
    goBackAct->setEnabled(false);

    // should we clear user data?
    loginPage->setUp();
    loginPage->resetProgressBar();
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::goToChat(){
    // user is connected
    isConnected = true;

    // change the page
    stackedWidget->setCurrentWidget(chatArea);

    // adjust menu bar items' active state
    // i.e. 'go to previous page' should be greyed out when on the chat page
    disconnectAct->setEnabled(true);

    // give chatArea userdata info
    emit giveUserData(userData);
}

// this resizes the window after changing widgets/pages
bool MainWindow::event(QEvent *event){
    if(event->type() == QEvent::LayoutRequest){
        setFixedSize(stackedWidget->currentWidget()->size());
    }
    return QMainWindow::event(event);
}

void MainWindow::resize(int index){
    // this only affects the widgets and not the window
    QWidget* currWidget = stackedWidget->widget(index);
    //currWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    currWidget->adjustSize();

    adjustSize();
}

void MainWindow::getUserData(UserData* data){
    qDebug() << "MainWindow: got the userdata in mainwindow! username: " << *(data->username);
    userData = data;
}

void MainWindow::setUpMenuBar(){

    // set up actions
    editEmoticonsAct = new QAction(tr("&edit emoticons"), this);
    goBackAct = new QAction(tr("&back to previous page"), this);
    disconnectAct = new QAction(tr("&disconnect from server"), this);

    connect(editEmoticonsAct, &QAction::triggered, this, &MainWindow::goToEditEmoticons);
    connect(goBackAct, &QAction::triggered, this, &MainWindow::goBackToPrevPage);
    connect(disconnectAct, &QAction::triggered, this, &MainWindow::disconnectFromServer);

    // set up menu
    optionMenu = new QMenu(tr("&options"), this);
    optionMenu->addAction(editEmoticonsAct);
    optionMenu->addAction(goBackAct);
    optionMenu->addAction(disconnectAct);

    // on login page, 'go back' and 'disconnect' should be greyed out
    goBackAct->setEnabled(false);
    disconnectAct->setEnabled(false);

    menuBar()->addMenu(optionMenu);
}

void MainWindow::setUpEmoticons(){
    // TODO: check for a local json as well?
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
       "  \"funny\": [           "
       "      \"¯\\\\_(ツ)_/¯\",  "
       "      \"ʕ•ᴥ•ʔ\",         "
       "      \"( ͡° ͜ʖ ͡°)\"      "
       "   ]"
       "}"
    );
    QByteArray emoticonData = defaultEmoticons.toUtf8();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(emoticonData));
    this->emoticonData = new QJsonObject(jsonDoc.object()); // allocate a new object with the same data
}

// slots for handling menu bar actions
void MainWindow::goToEditEmoticons(){
    // make sure goBackToPrevPage is enabled
    goBackAct->setEnabled(true);
    stackedWidget->setCurrentWidget(emoticonEdit);
}

void MainWindow::goBackToPrevPage(){
    // set current page to either the login if not connected yet,
    // or to the chat area if connected
    if(stackedWidget->currentIndex() == 2 && !isConnected){
        // if we're currently on the edit emoticons page and we're not connected
        // to a server, go back to login page
        stackedWidget->setCurrentWidget(loginPage);
    }else if(stackedWidget->currentIndex() == 2 && isConnected){
        stackedWidget->setCurrentWidget(chatArea);
    }
}

void MainWindow::disconnectFromServer(){
    // disconnect from the server
    QJsonObject byeMsg;
    byeMsg["Msg"] = "";
    byeMsg["MsgType"] = "3"; // 3 == Goodbye
    byeMsg["Sender"] = *userData->username;
    byeMsg["Timestamp"] = "";

    QJsonDocument doc(byeMsg);
    QByteArray msgBytes = doc.toJson();

    // send size of msg to expect first
    const char msgSize = int(msgBytes.length());

    qint64 msgSizeBytesWritten = clientSocket->write(&msgSize, 1);
    if(msgSizeBytesWritten == -1){
        qDebug() << "mainwindow: there was an error writing to the socket!";
    }

    qint64 bytesWritten = clientSocket->write(msgBytes, msgBytes.length());

    if(bytesWritten == -1){
        qDebug() << "mainwindow: there was an error writing to the socket!";
    }

    qDebug() << "disconnecting from host!";
    clientSocket->disconnectFromHost();

    // return to login page
    goToLogin();
}

MainWindow::~MainWindow()
{
    delete clientSocket;
    delete emoticonData;
    delete stackedWidget;
    delete userData;
    delete ui;
}
