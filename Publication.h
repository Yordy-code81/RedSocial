#ifndef PUBLICATION_H
#define PUBLICATION_H
#include<QString>

class Publication{
    QString title;
    QString desc;
    QString date;
    int id;
    int idU;
    int likes;
    int views;
public:
    Publication(int id, int idU, QString title, QString desc, QString date, int likes,int views=0)
        :title(title), desc(desc), date(date), id(id), idU(idU),likes(likes),views(views){}
    QString get_title(){
        return title;
    }
    QString get_desc(){
        return desc;
    }
    QString get_date(){
        return date;
    }
    int get_id(){
        return id;
    }
    int get_likes(){
        return likes;
    }
    int get_views(){
        return views;
    }
    int get_idU(){
        return idU;
    }
    void set_like(int like){
        this->likes=like;
    }
    void views_increase(){
        views++;
    }
};


#endif // PUBLICATION_H
