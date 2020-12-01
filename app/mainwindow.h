#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "login.h"
#include "chatarea.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QStackedWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void goToChat();
    void resize(int index);

private:
    Ui::MainWindow *ui;
    QStackedWidget* stackedWidget;
    Login* loginPage;
    ChatArea* chatArea;
    QTcpSocket* clientSocket;
};

#endif // MAINWINDOW_H
