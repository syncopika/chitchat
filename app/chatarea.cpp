#include "chatarea.h"
#include "ui_chatarea.h"

ChatArea::ChatArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatArea)
{
    ui->setupUi(this);
}

ChatArea::~ChatArea()
{
    delete ui;
}
