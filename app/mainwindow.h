#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "login.h"
#include "chatarea.h"
#include "emoticonedit.h"
#include "userdata.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QJsonObject>
#include <QAction>
#include <QMenu>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void giveUserData(UserData* data);

private slots:
    void goToChat();
    void goToLogin();
    void resize(int index);
    void getUserData(UserData* data);

    // for menu actions
    void goToEditEmoticons();
    void goBackToPrevPage();
    void disconnectFromServer();

private:
    Ui::MainWindow *ui;
    QStackedWidget* stackedWidget;

    // pages
    Login* loginPage;
    ChatArea* chatArea;
    EmoticonEdit* emoticonEdit;

    QTcpSocket* clientSocket;
    UserData* userData;
    QJsonObject* emoticonData;

    QMenu* optionMenu;
    QAction* editEmoticonsAct;
    QAction* goBackAct;
    QAction* disconnectAct;

    bool isConnected; // if is connected to chat server

    void setUpMenuBar();
    void setUpEmoticons();

    bool event(QEvent *event) override;
};

#endif // MAINWINDOW_H
