#include "mainwindow.h"
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

    loginPage = new Login;
    chatArea = new ChatArea;

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(chatArea);
    stackedWidget->setCurrentWidget(loginPage);

    // receive signal from the login page to move to chat area
    connect(loginPage, SIGNAL(goToChat()), this, SLOT(goToChat()));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(resize(int)));
}

void MainWindow::goToChat(){
    // change the page
    stackedWidget->setCurrentWidget(chatArea);
}

void MainWindow::resize(int index){
    QWidget* currWidget = stackedWidget->widget(index);
    currWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    currWidget->adjustSize();
    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}
