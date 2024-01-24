#include "login.h"
#include "ui_login.h"
#include <QMessageBox>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

login::~login()
{
    delete ui;
}
//点确认
void login::on_LoginConfirm_clicked()
{
    //简易管理员登录功能，验证账号密码，正确，发送信号，关闭登录窗口
    if(ui->usernameLineEdit->text() == "admin" && ui->passwordLineEdit->text() == "123456"){
        emit sendData(true);
        this->close();
    }else{//错误，清空输入内容
        QMessageBox::warning(this,tr("登录失败"),tr("用户名或密码输入错误！"),QMessageBox::Ok);
        this->ui->usernameLineEdit->clear();
        this->ui->passwordLineEdit->clear();
        this->ui->usernameLineEdit->setFocus();
    }
}
//点取消，清空输入内容
void login::on_LoginCancel_clicked()
{
    this->ui->usernameLineEdit->clear();
    this->ui->passwordLineEdit->clear();
    this->ui->usernameLineEdit->setFocus();
}
