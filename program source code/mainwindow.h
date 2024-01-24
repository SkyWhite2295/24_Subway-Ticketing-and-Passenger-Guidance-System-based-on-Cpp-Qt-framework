#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include "subwaymap.h"
#include "mygraphicsview.h"
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include "login.h"
#include <QAction>
#include <QProcess>
#include "paymessagebox.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //普通模式相关
    void comboBoxIndexChanged_StartBox(int index);
    void comboBoxIndexChanged_EndBox(int index);
    void on_ConfirmButton_clicked();  
    //旅游模式相关
    void comboBoxIndexChanged_StartBox_T(int index);
    void on_ConfirmButton_T_clicked();
    //管理员登录相关
    void loginTriggered();
    void logoutTriggered();
    void receiveData(bool isPass);
    //文件管理相关
    void actionStationManageTriggered();
    void actionEdgeManageTriggered();
    void actionTouristAttractionManageTriggered();
    void notepadFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void notepadError(QProcess::ProcessError error);
    //支付窗口相关
    void on_PayButton_clicked();
    void on_PayButton_T_clicked();

signals:
    void sendFare(float);       //向支付窗口发送票价信息

private:
    Ui::MainWindow *ui;

    SubwayMap subwayMap;                            //内部地图
    int startStationID=1;                                        //起点站ID
    int endStationID=1;                                         //终点站ID
    QSet<int> destinationIDs;                           //多个目的地，用于旅游模式
    QString textToDisplay;                                 //显示路线
    QVector<int> stationIDsAlongPath;             //路线沿途站点ID
    QVector<QString> stationNamesAlongPath;//路线沿途站点名字
    QVector<QString> lineAlongPath;
    float fare=0.0f;                                                    //车费
    double distance=0.0;                                          //路程
    double timeCost=0.0;                                         //用时
    //绘制地图
    QGraphicsScene* scene;
    QRectF sceneRect;
    void drawGraph();
    //状态栏显示时间
    QLabel* currentTimeLabel=new QLabel;          // 时间存储
    QTimer* timer;                                                // 计时器
    void timeUpdate();                                         // 状态栏时间更新
};

#endif // MAINWINDOW_H
