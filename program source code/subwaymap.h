#ifndef SUBWAYMAP_H
#define SUBWAYMAP_H

#include <QString>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>
#include <QSet>
#include <fstream>
#include <algorithm>
#include <queue>
#include <QDebug>
#include <set>
#include <QDir>
#include <QQueue>

using namespace std;

//地铁站点类：站点ID、站名、经度、纬度
class Station {
public:
    int id;
    unordered_map<int, double> neighbors;  // Maps neighbor Station id to edge weight.
    QString name;
    double longitude;//x
    double latitude;//y

    Station() { id = -1; name = "station"; longitude = 0.0; latitude = 0.0;}
    Station(int _id, const QString& _name, double _longitude, double _latitude)
        : id(_id), name(_name), longitude(_longitude), latitude(_latitude) {}
};

//边类：ID、两端站点ID、本段路径长度、路径所属线路ID
class Edge {
public:
    int id;
    int source;
    int destination;
    double weight;
    int lineid;

    Edge() { id = -1; source = -1; destination = -1; weight = -1.0; lineid = -1;}
    Edge(int _id, int _source, int _destination, double _weight, int _lineid)//, const QColor& _color
        : id(_id), source(_source), destination(_destination), weight(_weight), lineid(_lineid) {}//, color(_color)
};

//景点类：ID、景点名称、景点所属地铁站ID
class TouristAttraction{
public:
    int id;
    QString name;
    int stationid;

    TouristAttraction() {id=-1; name="touristattraction"; stationid=-1;}
    TouristAttraction(int _id, QString& _name, int _stationid)
        : id(_id), name(_name), stationid(_stationid) {}
};

//地铁运行图类：
class SubwayMap {
public:
    //站点表、边表、站点邻接表、景点表
    unordered_map<int, Station> stations;
    unordered_map<int, Edge> edges;
    unordered_map<int, QVector<int>> edgeIdsByStation;

    unordered_map<int, TouristAttraction>touristattractions;

    SubwayMap();
    ~SubwayMap();
    //添加站点、添加边（同时维护邻接表）、添加景点
    void addStation(int id, const QString& name, double longitude, double latitude) {
        stations.emplace(id, Station(id, name, longitude, latitude));
    }
    void addEdge(int id, int source, int destination, double weight, int lineid) {//, const QColor& color
        Edge edge(id, source, destination, weight, lineid);//, color
        edges.emplace(id, edge);
        stations[source].neighbors.emplace(destination, weight);
        stations[destination].neighbors.emplace(source, weight);
        edgeIdsByStation[source].push_back(id);
        edgeIdsByStation[destination].push_back(id);
    }

    void addTouristAttraction(int id, QString& name, int stationid){
        touristattractions.emplace(id, TouristAttraction(id, name, stationid));
    }

    //bool createInitialSample();
    //将站点表和边表写入文件、从文件读取站点和边进站点表和边表
    //bool saveToFile(const QString& stationFile, const QString& edgeFile);
    bool loadFromFile(const QString& stationFile, const QString& edgeFile, const QString& touristattractionFile);

    QVector<int> shortestPath(int start, int end);//传入两站ID，获取最短路径沿途站点ID
    QVector<int> minTransfer(int start, int end);//传入两站ID，获取最少换乘路径沿途站点ID

    QVector<int> pathEdge(const QVector<int>& path);//传入站点ID数组，获取路径沿途边ID
    float getFare(const QVector<int>& pathedges);//传入边ID数组，获取路径票价
    int getEdgeLineId(int start, int end);//传入两相邻站ID，获取两相邻站点间的边所属线路ID
    double getPathLength(const QVector<int>& pathedges);//传入边ID数组，获取路径全长
    QVector<int> travelRoute(int start, const QSet<int>& destinationIds);//传入一个起点站ID和一个目的站ID数组，获取从一点出发访问多点最短路径沿途站点ID
    QVector<QString> getPathStationNames(const QVector<int>& path);//传入站点ID数组，获取各站点名字
    QVector<QString> getPathEdgeLineIDs(const QVector<int>& path);//传入各边所属线路ID，获取线路提示文本
    QVector<int> getPathEdgeLineIDs_int(const QVector<int>& path);//传入边ID数组，获取各边所属线路ID
    QVector<bool> transferAtThisStation(const QVector<int>& edgeLineIDs);//传入各边所属线路ID，获取是否此时换乘
};

#endif // SUBWAYMAP_H
