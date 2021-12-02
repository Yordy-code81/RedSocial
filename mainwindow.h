#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <registro.h>
#include <AVL.h>
#include <BST.h>
#include <ListaSimple.h>
#include <QVBoxLayout>
#include "Publication.h"
#include "Comment.h"

#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QLineEdit>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Registro r_win;

    Avl<CUsuario*,QString> *avl_name;
    Avl<CUsuario*,QString> *avl_username;
    Avl<CUsuario*,int> *avl_id_users;
    Avl<Publication*,QString> *avl_publications;
    Bst<Comment*,int> *bst_comments;
    CUsuario *logeado;
    CUsuario *buscado;
    QVBoxLayout *publicacion_general;
    QVBoxLayout *perfil_buscado;
    QVBoxLayout *publicacion_buscado;
    Lista<Publication*> all_publications;
    Lista<Publication*> lista_first_fifty;
    Lista<Publication*> buscado_publicaciones;
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void registrarse();
    void logearse();
    void actualizar_users();
    void actualizar_followers();
    void cerrar_sesion();
    void publicar();
    void stay_publics();
    void stay_interactions();
    void change_img_user();
    void search_user();
    void look_perfil();
    void remove(QLayout*);
    void back();
    void back2();
    void follow();
    void ordenar(Lista<Publication*> &a, function<QVariant(Publication*)>criterio);

    //inicio de sesion
    void entrar();
    void cerrar();

    void likes_buscado();
    void recientes_buscado();
    void destacado_buscado();

    void presionar(QPushButton *but, Publication *p);
    void comentar(QLineEdit *texto,QTextBrowser *comm, Publication *p);

    void cambiar();
    void buscar_titulo();
    void ver_publicacion();
    void regresar();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
