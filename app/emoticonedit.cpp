#include "emoticonedit.h"
#include "ui_emoticonedit.h"

#include <QJsonArray>

EmoticonEdit::EmoticonEdit(QWidget *parent, QJsonObject* emoticonData) :
    QWidget(parent),
    ui(new Ui::EmoticonEdit)
{
    ui->setupUi(this);

    setUp();

    this->emoticonData = emoticonData;
    setupEmoticons();
}

void EmoticonEdit::setUp(){
    QObject::connect(ui->emoticonCategories, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticons(const QString&)));
}

void EmoticonEdit::tearDown(){
    QObject::disconnect(ui->emoticonCategories, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticons(const QString&)));
}

void EmoticonEdit::setupEmoticons(){

    // set up combo box with emoticons
    QComboBox* dropdown = ui->emoticonCategories;
    foreach(const QString& key, emoticonData->keys()){
        dropdown->addItem(key);
    }

    // show the current emoticons for the selected key
    QString selected = dropdown->currentText();
    updateEmoticons(selected);
}


void EmoticonEdit::updateEmoticons(const QString& emoticonCategory){
    QJsonArray selectedEmoticons = emoticonData->value(emoticonCategory).toArray();

    ui->emoticonDisplay->clear();

    foreach(const QJsonValue& emote, selectedEmoticons){
        QString e = emote.toString();
        ui->emoticonDisplay->append(e);
        ui->emoticonDisplay->moveCursor(QTextCursor::End);
    }
}



EmoticonEdit::~EmoticonEdit()
{
    delete ui;
}
