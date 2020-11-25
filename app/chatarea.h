#ifndef CHATAREA_H
#define CHATAREA_H

#include <QWidget>

namespace Ui {
    class ChatArea;
}

class ChatArea : public QWidget
{
    Q_OBJECT

public:
    explicit ChatArea(QWidget *parent = nullptr);
    ~ChatArea();

private:
    Ui::ChatArea* ui;

signals:

public slots:
};

#endif // CHATAREA_H
