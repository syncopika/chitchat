#ifndef USERDATA_H
#define USERDATA_H

#include <QGraphicsScene>
#include <QString>
#include <QTcpSocket>

struct UserData {
    QString* username;
    QGraphicsScene* avatar;

    ~UserData(){
        if(username){
            qDebug() << "deleted username!";
            delete username;
        }

        if(avatar){
            qDebug() << "deleted avatar!";
            delete avatar;
        }
    }
};

#endif // USERDATA_H
