#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

signals:
    void sendData(bool);

private slots:
    void on_LoginConfirm_clicked();
    void on_LoginCancel_clicked();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
