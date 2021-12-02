#ifndef INICIARSESION_H
#define INICIARSESION_H

#include <QMainWindow>
#include <ListaSimple.h>
#include <Users.h>
namespace Ui {
class iniciarsesion;
}

class iniciarsesion : public QMainWindow
{
    Q_OBJECT

public:
    Lista_Simple<CUsuario*>*ptrRed;
    explicit iniciarsesion(QWidget *parent = nullptr);
    ~iniciarsesion();
private slots:
    void cerrar();
    void entrar();
    void ver_password();

private:
    Ui::iniciarsesion *ui;
};

#endif // INICIARSESION_H
