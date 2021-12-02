#ifndef USERS_H
#define USERS_H
#include <QString>
#include <QVector>
#include <QListWidget>
class CUsuario
{
    int id;
    QString email;
    QString name;
    QString date_register;
    QVector<QString>followers;

public:
    CUsuario(int id, QString  email, QString  name, QString date_register) :id(id), email(email), name(name),
        date_register(date_register){}
    ~CUsuario() {}

    int      get_id(){return id;}
    QString  get_email() { return email; }
    QString  get_name()     {return name;}
    QString  get_date_register() { return date_register; }

    void add_followers(QString follower){
        followers.push_back(follower);
    }
    void mostrar_followers(QListWidget *ventana){
        ventana->clear();
        for(int i=0;i<followers.size();i++){
            ventana->addItem(followers.at(i));
        }
    }

};

#endif // USERS_H
