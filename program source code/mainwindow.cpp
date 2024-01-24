#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QGraphicsEllipseItem>
#include <QToolBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //初始化界面
    ui->setupUi(this);
    ui->graphicsView->refresh();
    ui->ShowRoute->clear();
    ui->ShowRoute_T->clear();
    ui->SPButton->setChecked(true);
    ui->LTButton->setChecked(false);
    ui->SPButton_T->setChecked(true);
    ui->LTButton_T->setChecked(false);

    //加载地图信息
    this->subwayMap.loadFromFile("resources/SubwayMap/stationInfo.txt", "resources/SubwayMap/edgeInfo.txt", "resources/SubwayMap/touristattractionInfo.txt");

    //将站点名称排序后置入ComboBox中
    QVector<QString> stationNames;
    for (const auto& stationPair : subwayMap.stations) {
        const Station& station = stationPair.second;
        stationNames.push_back(station.name);
    }
    sort(stationNames.begin(), stationNames.end());

    for (const QString& name : stationNames) {
        ui->StartBox->addItem(name);
        ui->EndBox->addItem(name);
        ui->StartBox_T->addItem(name);
    }

    //将景点名称排序后置入ScrollArea中
    QVector<QString> touristattractionNames;
    for (const auto& touristattractionPair : subwayMap.touristattractions) {
        const TouristAttraction& touristattraction = touristattractionPair.second;
        touristattractionNames.push_back(touristattraction.name);
    }
    sort(touristattractionNames.begin(), touristattractionNames.end());

    for (const QString& name : touristattractionNames) {
        QCheckBox* cb=new QCheckBox(name);
        ui->scrollAreaWidgetContents->layout()->addWidget(cb);
    }

    //设置ComboBox初始提示语
    ui->StartBox->setEditable(true);
    ui->EndBox->setEditable(true);
    ui->StartBox_T->setEditable(true);
    //ui->EndBox->lineEdit()->setPlaceholderText("select");
    ui->StartBox->setCurrentText("Select a station");
    ui->EndBox->setCurrentText("Select a station");
    ui->StartBox_T->setCurrentText("Select a station");

    //绘制地图
    this->scene = new QGraphicsScene;
    this->scene->setSceneRect(this->sceneRect);
    this->ui->graphicsView->setRenderHint(QPainter::Antialiasing);     // 精致绘图
    this->ui->graphicsView->setScene(this->scene);                            // 添加绘图场景
    this->ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);// 设置鼠标拖拽格式
    this->drawGraph();

    //连接信号和槽
    //更改起点终点选择
    connect(ui->StartBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::comboBoxIndexChanged_StartBox);
    connect(ui->EndBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::comboBoxIndexChanged_EndBox);
    connect(ui->StartBox_T, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::comboBoxIndexChanged_StartBox_T);
    //登录登出操作
    connect(ui->actionLog_in, SIGNAL(triggered()),this,SLOT(loginTriggered()));
    connect(ui->actionLog_out, SIGNAL(triggered()),this,SLOT(logoutTriggered()));
    //文件管理
    connect(ui->actionStation_Manage, SIGNAL(triggered()),this,SLOT(actionStationManageTriggered()));
    connect(ui->actionEdge_Manage, SIGNAL(triggered()),this,SLOT(actionEdgeManageTriggered()));
    connect(ui->actionTourist_Attraction_Manage, SIGNAL(triggered()),this,SLOT(actionTouristAttractionManageTriggered()));

    //帮助
    connect(this->ui->actionIntroduction, &QAction::triggered, [=]() {
        QString helpInfo = "程序功能基本介绍:\n";
        helpInfo += "****操作面板分普通模式与旅游模式\n";
        helpInfo += "****普通模式下，输入或选择起止站点，选择最短路径或最少换乘，\n";
        helpInfo += "点击确定，输出票价，并以文本和地图标注方式输出路线\n";
        helpInfo += "****旅游模式下，输入或选择起点，勾选一个或多个景点。勾选一个\n";
        helpInfo += "景点时，可选最短路径或最少换乘，与普通模式相同；勾选多个景点\n";
        helpInfo += "时，提供最短路线，并按全程的9折计费\n";
        helpInfo += "****地图支持鼠标拖动，鼠标滚轮缩放。鼠标指针在站点上或边上，\n";
        helpInfo += "可显示站点信息或线路信息\n";
        helpInfo += "****编辑(E)：提供简易管理员登录功能，登录后可选择管理站点、边\n";
        helpInfo += "和景点的配置文件，程序重启后更新被应用。未登录或登出后此功能\n";
        helpInfo += "被禁用（用户名：admin, 密码：123456）\n";
        helpInfo += "****状态栏可显示当前时间\n";
        helpInfo += "****************************************************************\n";
        QMessageBox::information(this, "基本介绍", helpInfo);
        });

    //关于
    connect(this->ui->actionAbout, &QAction::triggered, [=]() {
        QString helpInfo = "关于此项目:\n";
        helpInfo += "时间：2023.9 - 2023.12\n";
        helpInfo += "数据结构课设\n";
        helpInfo += "开发工具：Qt Creator 4.5.1 (Community)；Qt 5.10.1\n";
        helpInfo += "开发者：210702班 21074220 白昊天\n";
        QMessageBox::information(this, "关于此项目", helpInfo);
        });

    //状态栏
    //显示时间
    this->timer = new QTimer;
    connect(this->timer, &QTimer::timeout, this, &MainWindow::timeUpdate);
    this->timer->start(1000);
    //显示作者
    QLabel* author = new QLabel("author: 21074220 白昊天");
    this->ui->statusBar->addWidget(author);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//点击确认键，计算路径，显示票价和沿途站点id
void MainWindow::on_ConfirmButton_clicked()
{
    stationIDsAlongPath.clear();
    textToDisplay.clear();
    stationNamesAlongPath.clear();
    lineAlongPath.clear();

    if(ui->SPButton->isChecked()){//选择最短路径
        stationIDsAlongPath=subwayMap.shortestPath(startStationID,endStationID);
        //stationIDsAlongPath=subwayMap.pathEdge(subwayMap.shortestPath(startStationID,endStationID));
    }else if(ui->LTButton->isChecked()){//选择最少换乘
        stationIDsAlongPath=subwayMap.minTransfer(startStationID,endStationID);
        //stationIDsAlongPath=subwayMap.pathEdge(subwayMap.minTransfer(startStationID,endStationID));
    }
    //显示路线
    stationNamesAlongPath=subwayMap.getPathStationNames(stationIDsAlongPath);
    lineAlongPath=subwayMap.getPathEdgeLineIDs(subwayMap.pathEdge(stationIDsAlongPath));
    for (int i = 0; i < stationIDsAlongPath.size(); i++) {
        //textToDisplay += QString::number(stationIDsAlongPath[i]) + "\n";
        textToDisplay += stationNamesAlongPath[i] + "\n";
        textToDisplay += lineAlongPath[i];
    }
    ui->ShowRoute->clear();
    ui->ShowRoute->setText(textToDisplay);
    //计算车费，显示
    fare=0.0f;
    fare=subwayMap.getFare(subwayMap.pathEdge(stationIDsAlongPath));
    ui->ShowFare->clear();
    ui->ShowFare->setText(QString::number(fare));

    distance=subwayMap.getPathLength(subwayMap.pathEdge(stationIDsAlongPath));
    ui->ShowDistance->clear();
    ui->ShowDistance->setText(QString::number(distance));

    int transferNum=0;
    QVector<bool> transfer=subwayMap.transferAtThisStation(subwayMap.getPathEdgeLineIDs_int(subwayMap.pathEdge(stationIDsAlongPath)));
    for(bool t: transfer){
        if(t){
            transferNum++;
        }
    }
    timeCost=3.0*(stationIDsAlongPath.size()-1)+5.0*transferNum;
    ui->ShowTimeCost->clear();
    ui->ShowTimeCost->setText(QString::number(timeCost));
    //地图上标出路线
    ui->graphicsView->refresh();
    this->drawGraph();
    for(int i: stationIDsAlongPath){
        Station station=subwayMap.stations[i];
        QPoint point((int)station.longitude,(int)station.latitude);
        QGraphicsEllipseItem* stationItem = new QGraphicsEllipseItem;
        stationItem->setRect(-3, -3, 3 << 1, 3 << 1);
        stationItem->setPos(point);
        stationItem->setPen(QColor(Qt::red));
        stationItem->setBrush(QColor(Qt::red));
        this->scene->addItem(stationItem);
    }

}

//改变起点站
void MainWindow::comboBoxIndexChanged_StartBox(int index) {
    if (index >= 0) {
        QString selectedName = ui->StartBox->currentText();
        for (const auto& stationPair : subwayMap.stations) {
            const Station& station = stationPair.second;
            if (station.name == selectedName) {
                //int selectedId = station.id;
                //qDebug() << "start station ID: " << selectedId;
                startStationID=station.id;
                break;
            }
        }
    }
}

//改变终点站
void MainWindow::comboBoxIndexChanged_EndBox(int index) {
    if (index >= 0) {
        QString selectedName = ui->EndBox->currentText();
        for (const auto& stationPair : subwayMap.stations) {
            const Station& station = stationPair.second;
            if (station.name == selectedName) {
                //int selectedId = station.id;
                //qDebug() << "end station ID: " << selectedId;
                endStationID=station.id;
                break;
            }
        }
    }
}

//改变起点站(旅游模式)
void MainWindow::comboBoxIndexChanged_StartBox_T(int index) {
    if (index >= 0) {
        QString selectedName = ui->StartBox_T->currentText();
        for (const auto& stationPair : subwayMap.stations) {
            const Station& station = stationPair.second;
            if (station.name == selectedName) {
                //int selectedId = station.id;
                //qDebug() << "start station ID: " << selectedId;
                startStationID=station.id;
                break;
            }
        }
    }
}

//旅游模式，点击确认键
void MainWindow::on_ConfirmButton_T_clicked()
{
    destinationIDs.clear();
    fare=0.0f;

    //遍历哪些CheckBox被选中，确定景点名称
    QVector<QString> selectedDestinations;
    for (QObject* obj : ui->scrollAreaWidgetContents->children()) {
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(obj);
        if (checkBox && checkBox->isChecked()) {
            selectedDestinations.push_back(checkBox->text());
        }
    }

    //遍历选出的景点名称，确定景点所属站点id
    for(const QString& name :selectedDestinations){
        for (const auto& touristattractionPair : subwayMap.touristattractions) {
            const TouristAttraction& touristattraction = touristattractionPair.second;
            if (touristattraction.name == name) {
                destinationIDs.insert(touristattraction.stationid);
                break;
            }
        }
    }

    //若目的地只有一个，按普通模式方法处理
    if(destinationIDs.size()==1){
        endStationID=*destinationIDs.begin();
        stationIDsAlongPath.clear();
        textToDisplay.clear();
        stationNamesAlongPath.clear();
        lineAlongPath.clear();

        if(ui->SPButton_T->isChecked()){
            stationIDsAlongPath=subwayMap.shortestPath(startStationID,endStationID);
            //stationIDsAlongPath=subwayMap.pathEdge(subwayMap.shortestPath(startStationID,endStationID));
        }else if(ui->LTButton_T->isChecked()){
            stationIDsAlongPath=subwayMap.minTransfer(startStationID,endStationID);
            //stationIDsAlongPath=subwayMap.pathEdge(subwayMap.minTransfer(startStationID,endStationID));
        }

        stationNamesAlongPath=subwayMap.getPathStationNames(stationIDsAlongPath);
        lineAlongPath=subwayMap.getPathEdgeLineIDs(subwayMap.pathEdge(stationIDsAlongPath));
        for (int i = 0; i < stationIDsAlongPath.size(); i++) {
            //textToDisplay += QString::number(stationIDsAlongPath[i]) + "\n";
            textToDisplay += stationNamesAlongPath[i] + "\n";
            textToDisplay += lineAlongPath[i];
        }
        ui->ShowRoute_T->clear();
        ui->ShowRoute_T->setText(textToDisplay);
        fare=subwayMap.getFare(subwayMap.pathEdge(stationIDsAlongPath));
    }else if(destinationIDs.size()>1){//若目的地有多个，规划最短路线
        stationIDsAlongPath.clear();
        textToDisplay.clear();

        stationIDsAlongPath=subwayMap.travelRoute(startStationID,destinationIDs);

        stationNamesAlongPath=subwayMap.getPathStationNames(stationIDsAlongPath);
        lineAlongPath=subwayMap.getPathEdgeLineIDs(subwayMap.pathEdge(stationIDsAlongPath));
        for (int i = 0; i < stationIDsAlongPath.size(); i++) {
            //textToDisplay += QString::number(stationIDsAlongPath[i]) + "\n";
            textToDisplay += stationNamesAlongPath[i] + "\n";
            textToDisplay += lineAlongPath[i];
        }
        //旅游票价计算
        ui->ShowRoute_T->clear();
        ui->ShowRoute_T->setText(textToDisplay);
        fare=0.9*subwayMap.getFare(subwayMap.pathEdge(stationIDsAlongPath));
    }    
    ui->ShowFare_T->clear();
    ui->ShowFare_T->setText(QString::number(fare));
    //地图上标出路线
    ui->graphicsView->refresh();
    this->drawGraph();
    for(int i: stationIDsAlongPath){
        Station station=subwayMap.stations[i];
        QPoint point((int)station.longitude,(int)station.latitude);
        QGraphicsEllipseItem* stationItem = new QGraphicsEllipseItem;
        stationItem->setRect(-3, -3, 3 << 1, 3 << 1);
        stationItem->setPos(point);
        stationItem->setPen(QColor(Qt::red));
        stationItem->setBrush(QColor(Qt::red));
        this->scene->addItem(stationItem);
    }
}

//绘制地图
void MainWindow::drawGraph(){
    //根据站点坐标范围，确定绘图场景范围
    QVector<int> xList;
    QVector<int> yList;

    for (const auto& stationPair : subwayMap.stations) {
        const Station& station = stationPair.second;
        xList.push_back((int)(station.longitude));
        yList.push_back((int)(station.latitude));
    }

    int minX = *min_element(xList.begin(), xList.end()) - 500;
    int maxX = *max_element(xList.begin(), xList.end()) + 500;
    int minY = *min_element(yList.begin(), yList.end()) - 500;
    int maxY = *max_element(yList.begin(), yList.end()) + 500;

    this->sceneRect.setTopLeft(QPoint(0, 0));
    this->sceneRect.setBottomRight(QPoint((maxX - minX)* 3, (maxY - minY)* 3));

    //绘制边
    for (const auto& edgePair : subwayMap.edges) {
        const Edge& edge = edgePair.second;

        //获取边两端点
        Station stationA=subwayMap.stations.at(edge.source);
        Station stationB=subwayMap.stations.at(edge.destination);
        QPoint pointA((int)stationA.longitude, (int)stationA.latitude);
        QPoint pointB((int)stationB.longitude, (int)stationB.latitude);

        //编辑边提示信息
        QString tip= "Line: ";
        tip+=QString::number(edge.lineid);

        //根据所属线路确定边颜色
        QColor color;
        switch (edge.lineid) {
        case 1:
            color=QColor(QRgb(0xc23a30));
            break;
        case 2:
            color=QColor(QRgb(0x006098));
            break;
        case 4:
            color=QColor(QRgb(0x008e9c));
            break;
        case 5:
            color=QColor(QRgb(0xa6217f));
            break;
        case 6:
            color=QColor(QRgb(0xd29700));
            break;
        case 8:
            color=QColor(QRgb(0x009b6b));
            break;
        case 9:
            color=QColor(QRgb(0x8fc31f));
            break;
        case 10:
            color=QColor(QRgb(0x009bc0));
            break;
        case 13:
            color=QColor(QRgb(0xf9e700));
            break;
        default:
            color=QColor(Qt::black);
            break;
        }

        //绘制岀边
        QGraphicsLineItem* edgeItem = new QGraphicsLineItem;
        edgeItem->setPen(QPen(color));
        edgeItem->setCursor(Qt::PointingHandCursor);   // 设置光标
        edgeItem->setToolTip(tip);                                  // 添加注解
        edgeItem->setLine(QLineF(pointA, pointB));

        this->scene->addItem(edgeItem);
    }

    //绘制站点
    for (const auto& stationPair : subwayMap.stations) {
        const Station& station = stationPair.second;
        //获取站点坐标
        QPoint point((int)station.longitude,(int)station.latitude);
        //编辑站点提示信息
        QString tip="Name: "+station.name+"\n"
                +"ID: "+QString::number(station.id)+"\n"
                +"Location: "+QString::number(station.longitude)+", "+QString::number(station.latitude);

        //绘制岀站点
        QGraphicsEllipseItem* stationItem = new QGraphicsEllipseItem;
        stationItem->setRect(-5, -5, 5 << 1, 5 << 1);            // 设置站点圆形区域
        stationItem->setPos(point);                                      // 设置中心坐标
        stationItem->setPen(QColor(Qt::black));                    // 设置画笔颜色
        stationItem->setBrush(QColor(QRgb(0xffffff)));          // 设置填充
        stationItem->setCursor(Qt::PointingHandCursor);      // 设置鼠标形态
        stationItem->setToolTip(tip);                                     // 设置提示信息
        this->scene->addItem(stationItem);

        // 添加站点名字
        QGraphicsTextItem* textItem = new QGraphicsTextItem;
        textItem->setPlainText(station.name);                // 设置站点名字
        textItem->setFont(QFont("consolas", 5, 1));        // 设置名字格式
        textItem->setPos(point.x()+1, point.y()+1);         // 设置名字位置
        this->scene->addItem(textItem);
    }

}

//更新状态栏显示的时间
void MainWindow::timeUpdate()
{
    QDateTime CurrentTime = QDateTime::currentDateTime();
    QString Timestr = CurrentTime.toString("yyyy.MM.dd hh:mm:ss"); //设置显示的格式
    this->currentTimeLabel->setText(Timestr);
    this->ui->statusBar->addPermanentWidget(currentTimeLabel);
}

//点击登录
void MainWindow::loginTriggered(){
    login* l=new login;
    connect(l,SIGNAL(sendData(bool)),this,SLOT(receiveData(bool)));
    l->show();
}

//从登录界面接收是否登陆成功信号
void MainWindow::receiveData(bool isPass){
    if(isPass){
        ui->actionStation_Manage->setEnabled(true);
        ui->actionEdge_Manage->setEnabled(true);
        ui->actionTourist_Attraction_Manage->setEnabled(true);
        ui->actionLog_in->setEnabled(false);
    }
}

//点击登出
void MainWindow::logoutTriggered(){
    ui->actionStation_Manage->setEnabled(false);
    ui->actionEdge_Manage->setEnabled(false);
    ui->actionTourist_Attraction_Manage->setEnabled(false);
    ui->actionLog_in->setEnabled(true);
}

//点击站点文件管理
void MainWindow::actionStationManageTriggered(){
    QString filePath = "resources/SubwayMap/stationInfo.txt";

    // 创建QProcess对象
    QProcess *notepadProcess = new QProcess(this);

    // 将程序设置为“notepad.exe”，并添加文件路径作为参数
    notepadProcess->start("notepad.exe", QStringList() << filePath);

    connect(notepadProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(notepadFinished(int, QProcess::ExitStatus)));
    connect(notepadProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(notepadError(QProcess::ProcessError)));
}

//点击边文件管理
void MainWindow::actionEdgeManageTriggered(){
    QString filePath = "resources/SubwayMap/edgeInfo.txt";

    QProcess *notepadProcess = new QProcess(this);

    notepadProcess->start("notepad.exe", QStringList() << filePath);

    connect(notepadProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(notepadFinished(int, QProcess::ExitStatus)));
    connect(notepadProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(notepadError(QProcess::ProcessError)));
}

//点击景点文件管理
void MainWindow::actionTouristAttractionManageTriggered(){
    QString filePath = "resources/SubwayMap/touristattractionInfo.txt";

    QProcess *notepadProcess = new QProcess(this);

    notepadProcess->start("notepad.exe", QStringList() << filePath);

    connect(notepadProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(notepadFinished(int, QProcess::ExitStatus)));
    connect(notepadProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(notepadError(QProcess::ProcessError)));
}

void MainWindow::notepadFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Notepad process finished with exit code:" << exitCode << "Exit status:" << exitStatus;
}

void MainWindow::notepadError(QProcess::ProcessError error)
{
    qDebug() << "Notepad process error:" << error;
}

//点击支付按钮，弹出支付窗口
void MainWindow::on_PayButton_clicked()
{
    PayMessageBox* p=new PayMessageBox;
    connect(this,SIGNAL(sendFare(float)),p,SLOT(receiveFare(float)));
    emit sendFare(fare);
    p->show();
}

void MainWindow::on_PayButton_T_clicked()
{
    PayMessageBox* p=new PayMessageBox;
    connect(this,SIGNAL(sendFare(float)),p,SLOT(receiveFare(float)));
    emit sendFare(fare);
    p->show();
}
