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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void go();

private:
    Ui::MainWindow *ui;
    bool connectToServer(QString& username, QString& ipAddr, quint16 port, QTcpSocket* socket);
};

#endif // MAINWINDOW_H
