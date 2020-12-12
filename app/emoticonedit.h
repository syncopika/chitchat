#ifndef EMOTICONEDIT_H
#define EMOTICONEDIT_H

#include <QWidget>

namespace Ui {
class EmoticonEdit;
}

class EmoticonEdit : public QWidget
{
    Q_OBJECT

public:
    explicit EmoticonEdit(QWidget *parent = nullptr);
    ~EmoticonEdit();

private:
    Ui::EmoticonEdit *ui;
};

#endif // EMOTICONEDIT_H
