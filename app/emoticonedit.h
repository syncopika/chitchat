#ifndef EMOTICONEDIT_H
#define EMOTICONEDIT_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class EmoticonEdit;
}

class EmoticonEdit : public QWidget
{
    Q_OBJECT

public:
    explicit EmoticonEdit(QWidget *parent = nullptr, QJsonObject* emoticonData = nullptr);
    ~EmoticonEdit();

private:
    Ui::EmoticonEdit *ui;
    QJsonObject* emoticonData;

    void setUp();
    void tearDown();
    void setupEmoticons();

private slots:
    void updateEmoticons(const QString& emoticonCategory);
};

#endif // EMOTICONEDIT_H
