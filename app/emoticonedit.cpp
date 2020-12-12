#include "emoticonedit.h"
#include "ui_emoticonedit.h"

EmoticonEdit::EmoticonEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmoticonEdit)
{
    ui->setupUi(this);
}

EmoticonEdit::~EmoticonEdit()
{
    delete ui;
}
