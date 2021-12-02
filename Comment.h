#ifndef COMMENT_H
#define COMMENT_H
#include <QString>

class Comment{
    int id;
    int idPub;
    QString date;
    QString text;
public:
    Comment(int id,int idPub, QString date, QString text):id(id),idPub(idPub),date(date),text(text){}

    int get_id(){
        return id;
    }
    int get_idPub(){
        return idPub;
    }
    QString get_date(){
        return date;
    }
    QString get_text(){
        return text;
    }
};



#endif // COMMENT_H
