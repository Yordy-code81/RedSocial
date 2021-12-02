#include "registro.h"
#include "ui_registro.h"
#include <QFile>
#include <QTextStream>
int id=1000;
Registro::Registro(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Registro)
{
    ui->setupUi(this);
    connect(ui->Cancelar_Button,SIGNAL(released()),this,SLOT(cerrar()));
    connect(ui->Crear_Button,SIGNAL(released()),this,SLOT(registrar()));
}

Registro::~Registro()
{
    delete ui;
}

void Registro::registrar()
{
    id++;
    QString email=ui->Email_Text->text();
    QString name=ui->Name_Text->text();

    QFile file("users.tsv");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << "\n" << id << "\t" << email << "\t" << name << "\t" << "2020-11-26";
    }
    file.close();

    
    cerrar();
}

void Registro::cerrar()
{
    ui->Email_Text->setText("");
    ui->Name_Text->setText("");
    this->hide();
}


