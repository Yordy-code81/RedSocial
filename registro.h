#ifndef REGISTRO_H
#define REGISTRO_H

#include <QMainWindow>
#include <Users.h>
namespace Ui {
class Registro;
}

class Registro : public QMainWindow
{
    Q_OBJECT

public:
    explicit Registro(QWidget *parent = nullptr);
    ~Registro();
private slots:
    void cerrar();
    void registrar();

private:
    Ui::Registro *ui;

};

#endif // REGISTRO_H
