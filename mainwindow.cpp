#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <functional>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QFileDialog>
#include <QBuffer>
#include <QSettings>
#include <QMessageBox>
#include <QTextBrowser>
#include<QSignalMapper>
#include <QDate>
#include <QFontDatabase>

using namespace std;
bool searched=false;
bool show_register=false;
int id_publication;
int id_comments;
bool cambio=false;
QDate fecha=QDate::currentDate();
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_general->setCurrentIndex(0);
    connect(ui->Buscar_Button_title,SIGNAL(released()),this,SLOT(cambiar()));
    connect(ui->Buscar_Button_titulo,SIGNAL(released()),this,SLOT(buscar_titulo()));
    connect(ui->Regresar_Button,SIGNAL(released()),this,SLOT(regresar()));

    connect(ui->Entrar_Button,SIGNAL(released()),this,SLOT(logearse()));
    connect(ui->Cancelar_Button,SIGNAL(released()),this,SLOT(cerrar()));
    logeado=nullptr;
    buscado=nullptr;

    connect(ui->Register_Button,SIGNAL(released()),this,SLOT(registrarse()));
    connect(ui->Login_Button,SIGNAL(released()),this,SLOT(entrar()));
    connect(ui->Logout_Button,SIGNAL(released()),this,SLOT(cerrar_sesion()));
    connect(ui->Publicar_Button,SIGNAL(released()),this,SLOT(publicar()));
    connect(ui->Perfil_Button,SIGNAL(released()),this,SLOT(change_img_user()));
    avl_name=new Avl<CUsuario*,QString>([](CUsuario *a) {return a->get_name(); });
    avl_username=new Avl<CUsuario*,QString>([](CUsuario *a) {return a->get_email(); });
    avl_id_users=new Avl<CUsuario*,int>([](CUsuario *a) {return a->get_id(); });
    avl_publications=new Avl<Publication*,QString>([](Publication *a){return a->get_title();});
    bst_comments=new Bst<Comment*,int>([](Comment *a){return a->get_idPub();});
    publicacion_general=new QVBoxLayout();
    ui->scrollAreaWidgetContents->setLayout(publicacion_general);
    actualizar_users();
    actualizar_followers();
    stay_publics();
    stay_interactions();
    connect(ui->Buscar_Button,SIGNAL(released()),this,SLOT(search_user()));
    connect(ui->VerPerfil_Button,SIGNAL(released()),this,SLOT(look_perfil()));
    connect(ui->Follow_Button,SIGNAL(released()),this,SLOT(follow()));
    perfil_buscado=new QVBoxLayout();
    ui->scrollAreaWidgetContents_likes->setLayout(perfil_buscado);
    connect(ui->Back_Button,SIGNAL(released()),this,SLOT(back()));

    connect(ui->Recientes_Button,SIGNAL(released()),this,SLOT(recientes_buscado()));
    connect(ui->Likes_Button,SIGNAL(released()),this,SLOT(likes_buscado()));
    connect(ui->Destacadas_Button,SIGNAL(released()),this,SLOT(destacado_buscado()));

    publicacion_buscado = new QVBoxLayout();
    ui->scrollAreaWidgetContents_pub_buscado->setLayout(publicacion_buscado);
    connect(ui->Ver_button,SIGNAL(released()),this,SLOT(ver_publicacion()));
    connect(ui->Back_Button_2,SIGNAL(released()),this,SLOT(back2()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::actualizar_users()
{

    QFile file ("users.tsv");
    QString aux2,aux3,aux4,line;
    int aux1;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {

            QTextStream stream(&file);
            while(!stream.atEnd())
            {
               QString line=stream.readLine();
               QStringList data=line.split("\t");
               aux1=data[0].toInt();
               aux2=data[1];
               aux3=data[2];
               aux4=data[3];
               avl_username->add(new CUsuario(aux1,aux2,aux3,aux4));
               avl_name->add(new CUsuario(aux1,aux2,aux3,aux4));
               avl_id_users->add(new CUsuario(aux1,aux2,aux3,aux4));
            }
    }
    file.close();
}

void MainWindow::actualizar_followers(){
    QFile file ("followers.tsv");
    int aux1,aux2;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {

            QTextStream stream(&file);
            while(!stream.atEnd())
            {
                QString line=stream.readLine();
                QStringList data=line.split(",");
                aux1=data[0].toInt();
                aux2=data[1].toInt();
                CUsuario *user=avl_id_users->find(aux2);
                CUsuario *follower=avl_id_users->find(aux1);
                avl_id_users->find(user->get_id())->add_followers(follower->get_name());
                avl_name->find(user->get_name())->add_followers(follower->get_name());
                avl_username->find(user->get_email())->add_followers(follower->get_name());
            }
    }
    file.close();
}

void MainWindow::registrarse()
{
    avl_username->clear();
    avl_name->clear();
    avl_id_users->clear();
    r_win.show();
    show_register=true;
}
void MainWindow::logearse()
{
    QString username=ui->Username_Text->text();
    logeado=avl_username->find(username);
    if(logeado==nullptr){
        QMessageBox::warning(this,"Error","usuario no registrado");
    }else{


        ui->stackedWidget->setCurrentIndex(2);
        ui->Username_Text->setText("");

        ui->lbluser->setText("Bienvenido " + logeado->get_name());


        logeado->mostrar_followers(ui->Widget_Followers);


        QPixmap *img_main=new QPixmap(":/img/imagenes/usuario.png");
        ui->img_user->setPixmap(*img_main);

        QSettings settings("Save state", "GUIApp");

        settings.beginGroup("MainWindow");
        QByteArray image = settings.value(logeado->get_name()).toByteArray();
        if (!image.isNull()) {
        QPixmap pixmap;
        if (pixmap.loadFromData(image)) {
        this->ui->img_user->setPixmap(pixmap);
        }
        }
    }

}

void MainWindow::cerrar_sesion(){

    logeado=nullptr;
    ui->stackedWidget->setCurrentIndex(0);
    ui->Widget_Followers->clear();
}

void MainWindow::ordenar(Lista<Publication*> &a, function<QVariant(Publication*)>criterio){
     for (int i = 1; i < a.size(); ++i) {
         Publication *e = a.get(i);
         int j = i;
         for (; j >= 1 && criterio(a.get(j - 1)) > criterio(e); --j) {
             a.editpos(j,a.get(j - 1)); //= a.get(j - 1);
         }
         if (j != i) a.editpos(j,e); //= e;
     }

}

void MainWindow::publicar(){
    if(ui->txt_titulo->text()!="" && ui->txt_publicar->text()!="")
        {
            id_publication++;
            QString title=ui->txt_titulo->text();
            QLabel *titulo=new QLabel();
            titulo->setText(logeado->get_name()+":  "+title+"            "+fecha.toString("yyyy-MM-dd"));
            QTextBrowser *texto=new QTextBrowser();
            QString pubi=ui->txt_publicar->text();
            texto->setText(pubi);
            QPushButton *button=new QPushButton();
            button->setText("like");
            Publication *new_pub=new Publication(id_publication,logeado->get_id(),title,pubi,fecha.toString("yyyy-MM-dd"),0);

            QLineEdit *text_comment=new QLineEdit();
            QPushButton *but_comment=new QPushButton();
            but_comment->setText("Comentar");
            QTextBrowser *comentarios=new QTextBrowser();

            connect(button, &QPushButton::pressed, [this, button, new_pub] { presionar(button, new_pub); });;
            connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, new_pub] { comentar(text_comment,comentarios,new_pub); });


            publicacion_general->addWidget(titulo);
            publicacion_general->addWidget(texto);
            publicacion_general->addWidget(button);

            publicacion_general->addWidget(text_comment);
            publicacion_general->addWidget(but_comment);
            publicacion_general->addWidget(comentarios);

            ui->txt_titulo->setText("");
            ui->txt_publicar->setText("");
            all_publications.addlast(new_pub);
            QFile file("publications.tsv");

            if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                QTextStream stream(&file);
                stream << "\n" << id_publication << "\t" << logeado->get_id() << "\t" << title << "\t" << pubi << "\t" << fecha.toString("yyyy-MM-dd") << "\t" << 0;
            }
            file.close();
        }

}

void MainWindow::presionar(QPushButton *but, Publication *p){

    p->set_like(p->get_likes()+1);
    but->setText("like "+QString::number(p->get_likes()));
    QFile file("publications.tsv");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream stream(&file);
                stream << "id" << "\t" << "idU" << "\t" << "title" << "\t" << "desc" << "\t" <<"pubdate" << "\t" << "numlikes";
                for(int i=0;i<all_publications.size();i++){
                    if(all_publications.get(i)==p){
                        stream << "\n" << p->get_id() << "\t" << p->get_idU() << "\t" << p->get_title() << "\t" << p->get_desc() << "\t" << p->get_date() << "\t" << p->get_likes();
                        all_publications.get(i)->set_like(p->get_likes());
                    }
                    else{
                        stream << "\n" << all_publications.get(i)->get_id() << "\t" << all_publications.get(i)->get_idU() << "\t" << all_publications.get(i)->get_title() << "\t" << all_publications.get(i)->get_desc() << "\t" << all_publications.get(i)->get_date() << "\t" << all_publications.get(i)->get_likes();
                    }
                }

            }
            file.close();
    cambio=true;

}
void MainWindow::stay_publics(){


    QFile file ("publications.tsv");
        int aux1, aux2, aux6;
        QString aux3, aux4, aux5;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
                id_publication = -1;
                QTextStream stream(&file);
                while(!stream.atEnd())
                {
                    QString line=stream.readLine();
                    QStringList data=line.split("\t");
                    aux1=data[0].toInt();
                    aux2=data[1].toInt();
                    aux3=data[2];
                    aux4=data[3];
                    aux5=data[4];
                    aux6=data[5].toInt();
                    id_publication++;
                    if(id_publication!=0)
                    {
                        all_publications.addlast(new Publication(aux1,aux2,aux3,aux4,aux5,aux6));
                    }
                }
        }
        file.close();

        QFile fil ("comment.tsv");
            int au1, au2;
            QString au3, au4;
            if (fil.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                id_comments=-1;
                    QTextStream stream(&fil);
                    while(!stream.atEnd())
                    {
                        QString line=stream.readLine();
                        QStringList data=line.split("\t");
                        au1=data[0].toInt();
                        au2=data[1].toInt();
                        au3=data[2];
                        au4=data[3];
                        id_comments++;
                        if(id_comments!=0){
                            bst_comments->add(new Comment(au1,au2,au3,au4));
                        }
                    }
            }
            fil.close();

        for(int i=0;i<all_publications.size();i++)
        {
            lista_first_fifty.addlast(all_publications.get(i));
        }

        ordenar(lista_first_fifty,[](Publication* a){return a->get_likes();});

        for(int i = 0; i < 50; i++){

            Publication *publi= lista_first_fifty.get(lista_first_fifty.size()-1-i);
            int iduser=publi->get_idU();
            CUsuario *u=avl_id_users->find(iduser);
            QLabel *titulo=new QLabel();
            titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());

            QTextBrowser *texto=new QTextBrowser();
            texto->setText(publi->get_desc());
            QPushButton *button=new QPushButton();
            button->setText("like "+QString::number((publi->get_likes())));

            QLineEdit *text_comment=new QLineEdit();
            QPushButton *but_comment=new QPushButton();
            but_comment->setText("Comentar");
            QTextBrowser *comentarios=new QTextBrowser();

            QVector<Comment*> almacen;

            Comment *come=bst_comments->find(publi->get_id());
            while(come!=nullptr){
                almacen.push_back(come);
                comentarios->append("Anonimo: "+come->get_text());
                bst_comments->remove(come->get_idPub());
                come=bst_comments->find(publi->get_id());
            }

            connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
            connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

            publicacion_general->addWidget(titulo);
            publicacion_general->addWidget(texto);
            publicacion_general->addWidget(button);

            publicacion_general->addWidget(text_comment);
            publicacion_general->addWidget(but_comment);
            publicacion_general->addWidget(comentarios);

            publicacion_general->addSpacing(50);

            for(int i=0;i<almacen.size();i++){
                bst_comments->add(almacen.at(i));
            }
            almacen.clear();
        }
}

void MainWindow::stay_interactions(){
    QFile file("interaction.tsv");
    int idU, idP;
    QString date;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
            QTextStream stream(&file);
            while(!stream.atEnd())
            {
                QString line=stream.readLine();
                QStringList data=line.split("\t");
                idU=data[0].toInt();
                idP=data[1].toInt();
                date=data[2];
                if(data[3]=="TRUE") {
                    for(int i=0;i<all_publications.size();i++)
                    {
                        if(all_publications.get(i)->get_id()==idP)
                        {
                            all_publications.get(i)->views_increase();
                        }
                    }
                }
            }
    }
    file.close();
}

void MainWindow::comentar(QLineEdit *texto,QTextBrowser *comm, Publication *p){
    if(texto->text()!=""){
        QString comentario= texto->text();
        comm->append("Anonimo: "+comentario);
        QFile file("comment.tsv");
        id_comments++;

        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QTextStream stream(&file);
            stream << "\n" << id_comments << "\t" << p->get_id() << "\t" << fecha.toString("yyyy-MM-dd") << "\t" << comentario;
        }
        file.close();
        bst_comments->add(new Comment(id_comments,p->get_id(),fecha.toString("yyyy-MM-dd"),comentario));
        texto->setText("");
    }
    cambio=true;
}

void MainWindow::change_img_user(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"C:\\",tr("Image Files(*.png *.jpg *bmp)"));
        QPixmap* imagen = new QPixmap(filename);
        ui->img_user->setPixmap(*imagen);




        if (this->ui->img_user->pixmap() != nullptr) {
        QByteArray bArray;
        QBuffer buffer(&bArray);
        buffer.open(QIODevice::WriteOnly);
        this->ui->img_user->pixmap()->save(&buffer, "PNG");

        QSettings settings("Save state", "GUIApp");
        settings.beginGroup("MainWindow");
        settings.setValue(logeado->get_name(), bArray);
        }

}

void MainWindow::search_user(){
    ui->Widget_Search_Friends->clear();
    QString name=ui->Buscar_text->text();
    CUsuario *user=avl_name->find(name);
    if(user==nullptr){
        QMessageBox::warning(this,"Error","usuario no encontrado");
        searched=false;
    }else{
        searched=true;
        ui->Widget_Search_Friends->addItem(user->get_name());
        user=nullptr;
    }
}

void MainWindow::look_perfil(){
    if(searched){
        ui->stackedWidget->setCurrentIndex(3);
        QListWidgetItem *item = ui->Widget_Search_Friends->currentItem();
        QString fullname=item->text();
        ui->lbluser_searched->setText(fullname);

        QPixmap *img_main=new QPixmap(":/img/imagenes/usuario.png");
        ui->img_user_searched->setPixmap(*img_main);

        QSettings settings("Save state", "GUIApp");

        settings.beginGroup("MainWindow");
        QByteArray image = settings.value(fullname).toByteArray();
        if (!image.isNull()) {
        QPixmap pixmap;
        if (pixmap.loadFromData(image)) {
        this->ui->img_user_searched->setPixmap(pixmap);
        }
        }
        ui->stackedWidget_buscado->setCurrentIndex(2);
        ui->Buscar_text->setText("");
        ui->Widget_Search_Friends->clear();

        cambio=false;


        while(buscado_publicaciones.size()>0){
            buscado_publicaciones.removelast();
        }

        buscado=avl_name->find(ui->lbluser_searched->text());


        for(int i=0;i<all_publications.size();i++){
            if(all_publications.get(i)->get_idU()==buscado->get_id()){
                buscado_publicaciones.addlast(all_publications.get(i));
            }
        }
    }
}


void MainWindow::recientes_buscado(){
    ui->Likes_Button->setEnabled(true);
    ui->Recientes_Button->setEnabled(false);
    ui->Destacadas_Button->setEnabled(true);

    remove(perfil_buscado);

        ordenar(buscado_publicaciones, [](Publication* a){return a->get_date();});
        int size_buscado_publicaciones=buscado_publicaciones.size();
        if(buscado_publicaciones.size()<=50){
            for(int i=0;i<size_buscado_publicaciones;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }else{
            for(int i=0;i<50;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }

}

void MainWindow::destacado_buscado(){
    ui->Likes_Button->setEnabled(true);
    ui->Recientes_Button->setEnabled(true);
    ui->Destacadas_Button->setEnabled(false);
    remove(perfil_buscado);

        ordenar(buscado_publicaciones,[](Publication* a){return a->get_views();});
        int size_buscado_publicaciones=buscado_publicaciones.size();
        if(buscado_publicaciones.size()<=50){
            for(int i=0;i<size_buscado_publicaciones;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(" ͡❛ ͜ʖ ͡❛"+QString::number(publi->get_views())+" "+u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }else{
            for(int i=0;i<50;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }


}

void MainWindow::likes_buscado(){
    ui->Likes_Button->setEnabled(false);
    ui->Recientes_Button->setEnabled(true);
    ui->Destacadas_Button->setEnabled(true);

    remove(perfil_buscado);

        ordenar(buscado_publicaciones,[](Publication* a){return a->get_likes();});
        int size_buscado_publicaciones=buscado_publicaciones.size();
        if(buscado_publicaciones.size()<=50){
            for(int i=0;i<size_buscado_publicaciones;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }else{
            for(int i=0;i<50;i++){
                Publication *publi= buscado_publicaciones.get(buscado_publicaciones.size()-i-1);
                int iduser=publi->get_idU();
                CUsuario *u=avl_id_users->find(iduser);
                QLabel *titulo=new QLabel();
                titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
                QTextBrowser *texto=new QTextBrowser();
                texto->setText(publi->get_desc());
                QPushButton *button=new QPushButton();
                button->setText("like "+QString::number((publi->get_likes())));

                QLineEdit *text_comment=new QLineEdit();
                QPushButton *but_comment=new QPushButton();
                but_comment->setText("Comentar");
                QTextBrowser *comentarios=new QTextBrowser();

                QVector<Comment*> almacen;

                Comment *come=bst_comments->find(publi->get_id());
                while(come!=nullptr){
                    almacen.push_back(come);
                    comentarios->append("Anonimo: "+come->get_text());
                    bst_comments->remove(come->get_idPub());
                    come=bst_comments->find(publi->get_id());
                }

                connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
                connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

                perfil_buscado->addWidget(titulo);
                perfil_buscado->addWidget(texto);
                perfil_buscado->addWidget(button);

                perfil_buscado->addWidget(text_comment);
                perfil_buscado->addWidget(but_comment);
                perfil_buscado->addWidget(comentarios);

                perfil_buscado->addSpacing(50);

                for(int i=0;i<almacen.size();i++){
                    bst_comments->add(almacen.at(i));
                }
                almacen.clear();
            }
        }
}

void MainWindow::remove(QLayout* layout)
{
    QLayoutItem* child;
    while(layout->count()!=0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            remove(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}

void MainWindow::back2(){
ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::back(){
    ui->Likes_Button->setEnabled(true);
    ui->Recientes_Button->setEnabled(true);
    ui->Destacadas_Button->setEnabled(true);

    ui->stackedWidget->setCurrentIndex(2);
    ui->Follow_Button->setEnabled(true);
    buscado=nullptr;
    ui->stackedWidget_buscado->setCurrentIndex(0);

    while(buscado_publicaciones.size()>0){
        buscado_publicaciones.removelast();
    }
    remove(perfil_buscado);

    if(cambio){
        remove(publicacion_general);
        while(lista_first_fifty.size()>0){
            lista_first_fifty.removelast();
        }


        for(int i=0;i<all_publications.size();i++)
        {
            lista_first_fifty.addlast(all_publications.get(i));
        }

        ordenar(lista_first_fifty,[](Publication* a){return a->get_likes();});

        for(int i = 0; i < 50; i++){

            Publication *publi= lista_first_fifty.get(lista_first_fifty.size()-1-i);
            int iduser=publi->get_idU();
            CUsuario *u=avl_id_users->find(iduser);
            QLabel *titulo=new QLabel();
            titulo->setText(u->get_name()+":  "+publi->get_title()+ "          "+publi->get_date());
            QTextBrowser *texto=new QTextBrowser();
            texto->setText(publi->get_desc());
            QPushButton *button=new QPushButton();
            button->setText("like "+QString::number((publi->get_likes())));


            QLineEdit *text_comment=new QLineEdit();
            QPushButton *but_comment=new QPushButton();
            but_comment->setText("Comentar");
            QTextBrowser *comentarios=new QTextBrowser();

            QVector<Comment*> almacen;

            Comment *come=bst_comments->find(publi->get_id());
            while(come!=nullptr){
                almacen.push_back(come);
                comentarios->append("Anonimo: "+come->get_text());
                bst_comments->remove(come->get_idPub());
                come=bst_comments->find(publi->get_id());
            }

            connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
            connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

            publicacion_general->addWidget(titulo);
            publicacion_general->addWidget(texto);
            publicacion_general->addWidget(button);

            publicacion_general->addWidget(text_comment);
            publicacion_general->addWidget(but_comment);
            publicacion_general->addWidget(comentarios);

            for(int i=0;i<almacen.size();i++){
                bst_comments->add(almacen.at(i));
            }
            almacen.clear();

            cambio=false;
        }
    }
}

void MainWindow::follow(){
    CUsuario *buscado=avl_name->find(ui->lbluser_searched->text());
    QFile file("followers.tsv");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << "\n" << logeado->get_id() << "," << buscado->get_id();
    }
    file.close();
    ui->Follow_Button->setEnabled(false);
    avl_id_users->find(buscado->get_id())->add_followers(logeado->get_name());
    avl_name->find(buscado->get_name())->add_followers(logeado->get_name());
    avl_username->find(buscado->get_email())->add_followers(logeado->get_name());
}

void MainWindow::entrar(){
    if(show_register){

        actualizar_users();
        actualizar_followers();
        show_register=false;
    }
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::cerrar()
{

    ui->Username_Text->setText("");
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::cambiar(){
    avl_publications->clear();
    ui->stackedWidget_general->setCurrentIndex(1);
    for(int i=0;i<all_publications.size();i++){
        avl_publications->add(all_publications.get(i));
    }
}

void MainWindow::regresar(){
    ui->stackedWidget_general->setCurrentIndex(0);
}

void MainWindow::buscar_titulo(){
    ui->listWidget_titulos->clear();
    QString val=ui->Buscar_text_titulo->text();
    ui->Buscar_text_titulo->setText("");

    function<void(Publication*)> listar=[this, val](Publication *a){
        if(a->get_title().contains(val,Qt::CaseInsensitive)){
            CUsuario *user=avl_id_users->find(a->get_idU());
            ui->listWidget_titulos->addItem(user->get_name()+"\t: "+a->get_title());
        }
    };
    avl_publications->inOrder(listar);
}

void MainWindow::ver_publicacion(){
    ui->stackedWidget->setCurrentIndex(4);
    remove(publicacion_buscado);
    QStringList data=ui->listWidget_titulos->currentItem()->text().split("\t: ");
    ui->lblpub_searched->setText(data[0]);
    avl_publications->find(data[1])->views_increase();
    Publication *publi=avl_publications->find(data[1]);

    QFile file("interaction.tsv");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << "\n" << logeado->get_id() << "\t" << publi->get_id() << "\t" << fecha.toString("yyyy-MM-dd")<< "\t" << "TRUE";
    }

    QLabel *titulo=new QLabel();
    titulo->setText(ui->listWidget_titulos->currentItem()->text()+"            "+publi->get_date());
    QTextBrowser *texto=new QTextBrowser();
    texto->setText(publi->get_desc());
    QPushButton *button=new QPushButton();
    button->setText("like "+QString::number((publi->get_likes())));

    QLineEdit *text_comment=new QLineEdit();
    QPushButton *but_comment=new QPushButton();
    but_comment->setText("Comentar");
    QTextBrowser *comentarios=new QTextBrowser();

    QVector<Comment*> almacen;

    Comment *come=bst_comments->find(publi->get_id());
    while(come!=nullptr){
        almacen.push_back(come);
        comentarios->append("Anonimo: "+come->get_text());
        bst_comments->remove(come->get_idPub());
        come=bst_comments->find(publi->get_id());
    }

    connect(button, &QPushButton::pressed, [this, button, publi] { presionar(button, publi); });
    connect(but_comment, &QPushButton::pressed, [this, text_comment,comentarios, publi] { comentar(text_comment,comentarios,publi); });

    publicacion_buscado->addWidget(titulo);
    publicacion_buscado->addWidget(texto);
    publicacion_buscado->addWidget(button);

    publicacion_buscado->addWidget(text_comment);
    publicacion_buscado->addWidget(but_comment);
    publicacion_buscado->addWidget(comentarios);

    publicacion_buscado->addSpacing(50);

    for(int i=0;i<almacen.size();i++){
        bst_comments->add(almacen.at(i));
    }
    almacen.clear();
}
