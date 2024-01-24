#ifndef PAYMESSAGEBOX_H
#define PAYMESSAGEBOX_H

#include <QWidget>

namespace Ui {
class PayMessageBox;
}

class PayMessageBox : public QWidget
{
    Q_OBJECT

public:
    explicit PayMessageBox(QWidget *parent = 0);
    ~PayMessageBox();

private slots:
    void receiveFare(float fare);       //接收来自主窗口的票价信息

private:
    Ui::PayMessageBox *ui;
};

#endif // PAYMESSAGEBOX_H
