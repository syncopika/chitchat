#include "emoticonedit.h"
#include "ui_emoticonedit.h"

#include <QJsonArray>
#include <QtDebug>

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
    QObject::connect(ui->emoticonCategories, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticonDisplay(const QString&)));
    QObject::connect(ui->addEmoticon, SIGNAL(clicked()), this, SLOT(addNewEmoticon()));
}

void EmoticonEdit::tearDown(){
    QObject::disconnect(ui->emoticonCategories, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateEmoticonDisplay(const QString&)));
    QObject::disconnect(ui->addEmoticon, SIGNAL(clicked()), this, SLOT(addNewEmoticon()));
}

void EmoticonEdit::setupEmoticons(){
    // set up combo box with emoticons
    QComboBox* dropdown = ui->emoticonCategories;
    dropdown->clear();

    foreach(const QString& key, emoticonData->keys()){
        dropdown->addItem(key);
    }

    // show the current emoticons for the selected key
    QString selected = dropdown->currentText();
    updateEmoticonDisplay(selected);
}


void EmoticonEdit::updateEmoticonDisplay(const QString& emoticonCategory){
    QJsonArray selectedEmoticons = emoticonData->value(emoticonCategory).toArray();

    ui->emoticonDisplay->clear();

    foreach(const QJsonValue& emote, selectedEmoticons){
        QString e = emote.toString();
        ui->emoticonDisplay->append(e);
        ui->emoticonDisplay->moveCursor(QTextCursor::End);
    }
}

void EmoticonEdit::addNewEmoticon(){
    QString category = ui->addCategoryEdit->text().trimmed();
    QString emoticon = ui->addEmoticonEdit->text().trimmed();

    if(category != "" && emoticon != ""){
        if(emoticonData->contains(category)){
            // add new emoticon to existing category's emoticon list
            QJsonArray currCategoryEmoticons = emoticonData->value(category).toArray();
            currCategoryEmoticons.append(emoticon);
            emoticonData->insert(category, currCategoryEmoticons);

            // update UI if necessary
            QString selected = ui->emoticonCategories->currentText();
            updateEmoticonDisplay(selected);
        }else{
            // add a new category
            qDebug() << "adding a new category!";
            QStringList newCategory;
            newCategory.append(emoticon);
            QJsonArray categoryEmoticons = QJsonArray::fromStringList(newCategory);
            emoticonData->insert(category, categoryEmoticons);

            // make sure new key gets added to dropdown
            setupEmoticons();
        }

        // clear inputs
        ui->addCategoryEdit->clear();
        ui->addEmoticonEdit->clear();
    }
}



EmoticonEdit::~EmoticonEdit()
{
    delete ui;
}
