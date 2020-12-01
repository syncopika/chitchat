#ifndef CHATAREA_H
#define CHATAREA_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
    class ChatArea;
}

class ChatArea : public QWidget
{
    Q_OBJECT

public:
    explicit ChatArea(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~ChatArea();

private:
    Ui::ChatArea* ui;
    QTcpSocket* socket;
    QString messageInput;

signals:

private slots:
    void receiveMessage();
};

#endif // CHATAREA_H
