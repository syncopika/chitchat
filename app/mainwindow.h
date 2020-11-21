#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum MessageType {
        Hello = 1,   //1
        Message,     //2
        Goodbye      //3
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void go();

private:
    Ui::MainWindow *ui;
    bool connectToServer(QString& username, QString& ipAddr, quint16 port, QTcpSocket* socket);
    bool sendHelloMessage();
};

#endif // MAINWINDOW_H
