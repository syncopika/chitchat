#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}
