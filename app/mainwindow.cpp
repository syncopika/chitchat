#include "mainwindow.h"
#include "userdata.h"
#include "ui_mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set up qwidgets for the windows
    stackedWidget = new QStackedWidget;
    QMainWindow::setCentralWidget(stackedWidget);

    userData = nullptr;
    clientSocket = new QTcpSocket();
    loginPage = new Login(nullptr, clientSocket);
    chatArea = new ChatArea(nullptr, clientSocket);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(chatArea);
    stackedWidget->setCurrentWidget(loginPage);

    // receive signal from the login page to move to chat area
    connect(loginPage, SIGNAL(sendUserData(UserData*)), this, SLOT(getUserData(UserData*)));
    connect(loginPage, SIGNAL(goToChat()), this, SLOT(goToChat()));
    connect(this, SIGNAL(giveUserData(UserData*)), chatArea, SLOT(getUserData(UserData*)));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(resize(int)));
}

void MainWindow::goToLogin(){

}

void MainWindow::goToChat(){
    // change the page
    stackedWidget->setCurrentWidget(chatArea);

    // give chatArea userdata info
    emit giveUserData(userData);
}

void MainWindow::resize(int index){
    QWidget* currWidget = stackedWidget->widget(index);
    currWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    currWidget->adjustSize();
    adjustSize();
}

void MainWindow::getUserData(UserData* data){
    qDebug() << "MainWindow: got the userdata in mainwindow! username: " + *(data->username);
    userData = data;
}

MainWindow::~MainWindow()
{
    delete clientSocket;
    delete stackedWidget;
    delete userData;
    delete ui;
}
