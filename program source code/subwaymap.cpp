#include "subwaymap.h"
#include <QTextStream>

SubwayMap::SubwayMap(){
    this->stations.clear();
    this->edges.clear();
    this->edgeIdsByStation.clear();
    this->touristattractions.clear();
}

SubwayMap::~SubwayMap(){}

//----------------------------保存到文件----------------------------//
//bool SubwayMap::saveToFile(const QString& stationFile, const QString& edgeFile) {
//    // 打开站点信息文件进行写入
//    QFile stationOutputFile(stationFile);
//    if (!stationOutputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        qWarning() << "Failed to open station file for writing";
//        return false;
//    }

//    QTextStream stationTextStream(&stationOutputFile);

//    for (const auto& stationPair : stations) {
//        const Station& station = stationPair.second;
//        stationTextStream << station.id << " "
//                          << station.name << " "
//                          << station.longitude << " "
//                          << station.latitude << "\n";
//    }

//    stationOutputFile.close();

//    // 打开边信息文件进行写入
//    QFile edgeOutputFile(edgeFile);
//    if (!edgeOutputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//        qWarning() << "Failed to open edge file for writing";
//        return false;
//    }

//    QTextStream edgeTextStream(&edgeOutputFile);

//    for (const auto& edgePair : edges) {
//        const Edge& edge = edgePair.second;
//        edgeTextStream << edge.id << " "
//                       << edge.source << " "
//                       << edge.destination << " "
//                       << edge.weight << " "
//                       //<< edge.lineid << " "
//                       //<< edge.color.name() << "\n";
//                       << edge.lineid << "\n";
//    }

//    edgeOutputFile.close();
//    qWarning() << "saved";
//    return true;
//}

//----------------------------从文件读取数据----------------------------//
bool SubwayMap::loadFromFile(const QString& stationFile, const QString& edgeFile, const QString& touristattractionFile) {
    // Clear existing data in the maps
    stations.clear();
    edges.clear();
    touristattractions.clear();

    // 打开站点信息文件进行读取
    QFile stationInputFile(stationFile);
    if (!stationInputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open station file for reading";
        return false;
    }

    QTextStream stationTextStream(&stationInputFile);

    while (!stationTextStream.atEnd()) {
        int id;
        QString name;
        double longitude, latitude;
        stationTextStream >> id >> name >> longitude >> latitude;
        if(id!=0) this->addStation(id, name, longitude, latitude);
    }

    stationInputFile.close();

    // 打开边信息文件进行读取
    QFile edgeInputFile(edgeFile);
    if (!edgeInputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open edge file for reading";
        return false;
    }

    QTextStream edgeTextStream(&edgeInputFile);

    while (!edgeTextStream.atEnd()) {
        int id, source, destination, lineid;
        double weight;
        //QString colorStr;
        edgeTextStream >> id >> source >> destination >> weight >> lineid ;//>> colorStr
        //QColor color(colorStr);
        if(id!=0) this->addEdge(id, source, destination, weight, lineid);//, color
    }

    edgeInputFile.close();

    // 打开景点信息文件进行读取
    QFile touristattractionInputFile(touristattractionFile);
    if (!touristattractionInputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open touristattraction file for reading";
        return false;
    }

    QTextStream touristattractionTextStream(&touristattractionInputFile);

    while (!touristattractionTextStream.atEnd()) {
        int id, stationid;
        QString name;
        touristattractionTextStream >> id >> name >> stationid ;
        if(id!=0) this->addTouristAttraction(id, name, stationid);
    }

    touristattractionInputFile.close();

    return true;
}

//----------------------------两点间最短路径算法----------------------------//
QVector<int> SubwayMap::shortestPath(int start, int end){
    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    set<pair<double, int>> pq;

    for (const auto& StationPair : stations) {
        int id = StationPair.first;
        dist[id] = numeric_limits<double>::max();
        prev[id] = -1;
    }

    dist[start] = 0.0;
    pq.insert({ 0.0, start });

    while (!pq.empty()) {
        int u = pq.begin()->second;
        pq.erase(pq.begin());

        for (const auto& neighbor : stations[u].neighbors) {
            int v = neighbor.first;
            double weight = neighbor.second;

            if (dist[u] + weight < dist[v]) {
                pq.erase({ dist[v], v });
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.insert({ dist[v], v });
            }
        }
    }

    QVector<int> path;
    int current = end;
    while (current != -1) {
        path.push_back(current);
        current = prev[current];
    }

    reverse(path.begin(), path.end());
    return path;
}

//----------------------------两点间最少换乘算法----------------------------//
QVector<int> SubwayMap::minTransfer(int start, int end) {
    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    set<pair<double, int>> pq;

    for (const auto& StationPair : stations) {
        int id = StationPair.first;
        dist[id] = numeric_limits<double>::max();
        prev[id] = -1;
    }

    dist[start] = 0.0;
    pq.insert({ 0.0, start });

    while (!pq.empty()) {
        int u = pq.begin()->second;
        pq.erase(pq.begin());

        for (const auto& neighbor : stations[u].neighbors) {
            int v = neighbor.first;
            double weight = neighbor.second;

            // 如果换乘，则添加权重。
            double transferCost = (getEdgeLineId(u, v) != getEdgeLineId(prev[u], u)) ? 100.0 : 0;

            if (dist[u] + weight + transferCost < dist[v]) {
                pq.erase({ dist[v], v });
                dist[v] = dist[u] + weight + transferCost;
                prev[v] = u;
                pq.insert({ dist[v], v });
            }
        }
    }

    QVector<int> path;
    int current = end;
    while (current != -1) {
        path.push_back(current);
        current = prev[current];
    }
    reverse(path.begin(), path.end());
    return path;
}

int SubwayMap::getEdgeLineId(int start, int end){
    for (int edgeId : edgeIdsByStation[start]) {
        Edge edge = edges[edgeId];
        if ((edge.source == start && edge.destination == end) || (edge.source == end && edge.destination == start)) {
            return edge.lineid;
        }
    }
    //qWarning()<<"error in function getEdgeLineId";
    return -1;
}

//----------------------------获取路径上经过的边---------------------------//
QVector<int> SubwayMap::pathEdge(const QVector<int>& path){
    QVector<int> path_edge;
    for (int i = 0; i < path.size() - 1; i++) {
        int source = path[i];
        int destination = path[i + 1];
        for (int edgeId : edgeIdsByStation[source]) {
            Edge edge = edges[edgeId];
            if ((edge.source == source && edge.destination == destination) || (edge.source == destination && edge.destination == source)) {
                path_edge.push_back(edge.id);//lineid
                break;
            }
        }
    }
    return path_edge;
}

//----------------------------根据经过的边，计算总路程得出票价---------------------------//
float SubwayMap::getFare(const QVector<int>& pathedges){
    double distance=0.0;
    float fare=0.0f;
    for(int edgeId : pathedges){
        Edge edge = edges[edgeId];
        distance+=edge.weight;
    }
    if(distance<6){
        fare=3.0f;
    }else if (distance>=6 && distance<12) {
        fare=4.0f;
    }else if (distance>=12 && distance<22) {
        fare=5.0f;
    }else if (distance>=22 && distance<32) {
        fare=6.0f;
    }else if (distance>=32 && distance<52) {
        fare=7.0f;
    }else if (distance>=52 && distance<72) {
        fare=8.0f;
    }else if (distance>=72) {
        fare=9.0f;
    }

    return fare;
}

//----------------------------根据经过的边，计算总路程---------------------------//
double SubwayMap::getPathLength(const QVector<int>& pathedges){
    double distance=0.0;
    for(int edgeId : pathedges){
        Edge edge = edges[edgeId];
        distance+=edge.weight;
    }

    return distance;
}

//----------------------------从一点出发到多个目的地最短路径---------------------------//
QVector<int> SubwayMap::travelRoute(int start, const QSet<int>& destinationIds) {
    QVector<int> travelRoute;
    QSet<int> remainingDestinations = destinationIds;
    int currentStation = start;

    while (!remainingDestinations.empty()) {
        int nearestDestination = -1;
        QVector<int> shortestPathToNearest;
        double shortestDistance = numeric_limits<double>::max();
        // 查找距离当前车站最近的目的地
        for (int destination : remainingDestinations) {
            if (destination == currentStation) {
                continue;
            }

            QVector<int> path = shortestPath(currentStation, destination);
            QVector<int> pathedge=pathEdge(path);
            double pathLength = getPathLength(pathedge);

            if (pathLength < shortestDistance) {
                shortestDistance = pathLength;
                nearestDestination = destination;
                shortestPathToNearest = path;
            }
        }
        //已找到最近一个目的地，以此站为起点，在还未前往的目的地中再找最近一个目的地
        if (nearestDestination != -1) {
            travelRoute.append(shortestPathToNearest);
            currentStation = nearestDestination;
            remainingDestinations.remove(nearestDestination);
        } else {
            break;
        }
    }
    //中转站作为终点再作为起点，被记录了两次，删去一次
    for (auto it = travelRoute.begin(); it != travelRoute.end(); ) {
        if (it + 1 != travelRoute.end() && *it == *(it + 1)) {
            it = travelRoute.erase(it); // Remove one of the equal numbers
        } else {
            ++it;
        }
    }
    return travelRoute;
}

//----------------------------传入站点ID数组，获取各站点名字---------------------------//
QVector<QString> SubwayMap::getPathStationNames(const QVector<int>& path){
    QVector<QString> stationNames;
    for(int i: path){
        Station station=stations[i];
        stationNames.push_back("●"+station.name);
    }
    return stationNames;
}

//----------------------------传入各边所属线路ID，获取线路提示文本---------------------------//
QVector<QString> SubwayMap::getPathEdgeLineIDs(const QVector<int>& path){
    QVector<QString> edgeLineIDs_str;
    QVector<int> edgeLineIDs_int=getPathEdgeLineIDs_int(path);
    QVector<bool> transfer=transferAtThisStation(edgeLineIDs_int);
    for(int i=0; i<edgeLineIDs_int.size(); i++){
        if(transfer[i]){
            QString line="→！！！换乘："+QString::number(edgeLineIDs_int[i])+"号线"+"！！！\n";
            edgeLineIDs_str.push_back(line);
        }else {
            QString line="→"+QString::number(edgeLineIDs_int[i])+"号线"+"\n";
            edgeLineIDs_str.push_back(line);
        }
    }
    edgeLineIDs_str.push_back("------------------------");
    return edgeLineIDs_str;
}

//----------------------------传入边ID数组，获取各边所属线路ID---------------------------//
QVector<int> SubwayMap::getPathEdgeLineIDs_int(const QVector<int>& path){
    QVector<int> edgeLineIDs;
    for(int i: path){
        Edge edge=edges[i];
        int line=edge.lineid;
        edgeLineIDs.push_back(line);
    }
    return edgeLineIDs;
}

//----------------------------传入各边所属线路ID，获取是否此时换乘---------------------------//
QVector<bool> SubwayMap::transferAtThisStation(const QVector<int>& edgeLineIDs){
    QVector<bool> transfer;
    transfer.push_back(false);
    if(edgeLineIDs.size()<=1){
        return transfer;
    }
    int temp=edgeLineIDs[0];
    for(int i=1; i<edgeLineIDs.size(); i++){
        if(edgeLineIDs[i]==temp){
            transfer.push_back(false);
        }else {
            transfer.push_back(true);
            temp=edgeLineIDs[i];
        }
    }
    transfer.push_back(false);
    return transfer;
}
















