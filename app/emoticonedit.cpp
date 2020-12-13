#include "emoticonedit.h"
#include "ui_emoticonedit.h"

#include <QJsonArray>

EmoticonEdit::EmoticonEdit(QWidget *parent, QJsonObject* emoticonData) :
    QWidget(parent),
    ui(new Ui::EmoticonEdit)
{
    ui->setupUi(this);

    this->emoticonData = emoticonData;
    setupEmoticons();
}


void EmoticonEdit::setupEmoticons(){

    // set up combo box with emoticons
    QTextBrowser* categoryDisplay = ui->categoryDisplay;
    foreach(const QString& key, emoticonData->keys()){
        categoryDisplay->append(key);
        categoryDisplay->moveCursor(QTextCursor::End);
    }

    // show the current emoticons for the selected key (first category)
    // also highlight the first category (i.e. blue background?)
    QString firstCategory = emoticonData->keys()[0];
    updateEmoticons(firstCategory);
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
