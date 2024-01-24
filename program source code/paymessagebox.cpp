#include "paymessagebox.h"
#include "ui_paymessagebox.h"

PayMessageBox::PayMessageBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PayMessageBox)
{
    ui->setupUi(this);
}

PayMessageBox::~PayMessageBox()
{
    delete ui;
}

void PayMessageBox::receiveFare(float fare){
    ui->fare->setText(QString::number(fare));
}
