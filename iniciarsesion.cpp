#include "iniciarsesion.h"
#include "ui_iniciarsesion.h"

bool login=false;
iniciarsesion::iniciarsesion(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::iniciarsesion)
{
    ui->setupUi(this);
    connect(ui->Entrar_Button,SIGNAL(released()),this,SLOT(entrar()));
    connect(ui->Cancelar_Button,SIGNAL(released()),this,SLOT(cerrar()));
    connect(ui->visibility_Check,SIGNAL(released()),this,SLOT(ver_password()));

}

iniciarsesion::~iniciarsesion()
{
    delete ui;
}
void iniciarsesion::entrar()
{
    QString username=ui->Username_Text->text();
    QString password=ui->Password_Text->text();
    ptrRed->logearse(username,password);
    if(ptrRed->get_logeado())
    {
        cerrar();
        ui->label->setText("Inicie sesión");

    }
    else
    {
        ui->label->setText("Usuario o contraseña incorrecto");
    }

}
void iniciarsesion::cerrar()
{
    ui->Username_Text->setText("");
    ui->Password_Text->setText("");
    this->hide();
}
void iniciarsesion::ver_password()
{
    if(ui->visibility_Check->isChecked())
        ui->Password_Text->setEchoMode(QLineEdit::Normal);
    else
        ui->Password_Text->setEchoMode(QLineEdit::Password);
}
