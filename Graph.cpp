#include "Graph.hpp"
#include "tinyxml2.h"
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <stack>
#include <cmath>
#include <limits>
#include <map>

Graph::Graph(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    numVertices = 0;
    tinyxml2::XMLError err = doc.LoadFile(filename.c_str());
    if (err == tinyxml2::XML_ERROR_FILE_NOT_FOUND || err == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED) {
        std::cout << "Unable to open file: " << filename << std::endl;
        return;
    }
    
    if (err != tinyxml2::XML_SUCCESS) {
        std::cout << "Invalid format for file: " << filename << std::endl;
        return;
    }
    
    tinyxml2::XMLElement* root = doc.FirstChildElement("osm");

    for (tinyxml2::XMLElement* nodeElem = root->FirstChildElement("node"); nodeElem; nodeElem = nodeElem->NextSiblingElement("node")) {
        unsigned long id = nodeElem->Unsigned64Attribute("id");
        double lat = nodeElem->DoubleAttribute("lat");
        double lon = nodeElem->DoubleAttribute("lon");
        addVertex(id, lat, lon);
    }

    for (tinyxml2::XMLElement* wayElem = root->FirstChildElement("way"); wayElem; wayElem = wayElem->NextSiblingElement("way")) {
        bool isOneway = false;
        bool isHighway = false;
        std::string highwayType;
        for (tinyxml2::XMLElement* tag = wayElem->FirstChildElement("tag"); tag; tag = tag->NextSiblingElement("tag")) {
            const char* k = tag->Attribute("k");
            const char* v = tag->Attribute("v");
            if (k && v) {
                std::string key = k;
                std::string val = v;
                if (key == "highway") {
                    highwayType = val;
                    isHighway = true;
                }
                if (key == "oneway") {
                    if (val == "yes" || val == "1") {
                        isOneway = true;
                    }
                }
            }
        }

        if (!isHighway) {
            continue;
        }

        std::vector<unsigned long> way_nodes;
        for (tinyxml2::XMLElement* nd = wayElem->FirstChildElement("nd"); nd; nd = nd->NextSiblingElement("nd")) {
            way_nodes.push_back(nd->Unsigned64Attribute("ref"));
        }
        for (size_t i = 0; i + 1 < way_nodes.size(); ++i) {
            addEdge(way_nodes[i], way_nodes[i + 1], highwayType);
            if (!isOneway) {
                addEdge(way_nodes[i + 1], way_nodes[i], highwayType);
            }   
        }
    }
  
    removeIsolatedNodes();
    std::cout << "Graph OK" << std::endl;
}

void Graph::addVertex(unsigned long ID, double lat, double lon) {
    Vertex vertex = Vertex(ID, lat, lon);
    adjacencyList.push_back(vertex);
    vertexIndexMap[ID] = numVertices;
    numVertices++;
}

void Graph::removeVertex(unsigned long ID) {
    auto it = vertexIndexMap.find(ID);
    unsigned int index = it->second;

    for (const Edge& edge : adjacencyList[index].getEdges()) {
        unsigned long endId = edge.getEnd();
        auto endIt = vertexIndexMap.find(endId);
        unsigned int endIndex = endIt->second;
        std::list<Edge>& incoming = adjacencyList[endIndex].getIncoming();
        for (auto it2 = incoming.begin(); it2 != incoming.end(); ) {
            if (it2->getStart() == ID) {
                it2 = incoming.erase(it2);
            } else {
                ++it2;
            }
}
    }

    for (const Edge& edge : adjacencyList[index].getIncoming()) {
        unsigned long startId = edge.getStart();
        auto startIt = vertexIndexMap.find(startId);
        unsigned int startIndex = startIt->second;
        std::list<Edge>& leaving = adjacencyList[startIndex].getEdges();
        for (auto it2 = leaving.begin(); it2 != leaving.end(); ) {
            if (it2->getEnd() == ID) {
                it2 = leaving.erase(it2);  
            } else {
                ++it2;            
            }
        }
    }

    std::swap(adjacencyList[index], adjacencyList.back());
    adjacencyList.pop_back();
    vertexIndexMap.erase(ID);

    vertexIndexMap[adjacencyList[index].getId()] = index;
}

void Graph::addEdge(unsigned long start, unsigned long end, std::string highwayType) {
    auto Start = vertexIndexMap.find(start);
    auto End = vertexIndexMap.find(end);

    unsigned int index = Start->second;
    double distance = haversine(adjacencyList[index], adjacencyList[End->second], highwayType);
    adjacencyList[index].addEdge(Edge(start, end, distance, highwayType));
    index = End->second;
    adjacencyList[index].addIncoming(Edge(start, end, distance, highwayType));
}

void Graph::removeIsolatedNodes() {
    std::vector<unsigned long> toRemove;
    for (Vertex& vertex : adjacencyList) {
        if (vertex.getEdges().empty() && vertex.getIncoming().empty()) {
            toRemove.push_back(vertex.getId());
        }
    }
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        removeVertex(*it);
    }
}

const std::vector<Vertex>& Graph::getVertices() const {return adjacencyList;}

double Graph::haversine(const Vertex& start, const Vertex& end, const std::string& highwayType) const {
    static const double R = 6378137.0;

    double lat1 = start.getLatitude();
    double lon1 = start.getLongitude();
    double lat2 = end.getLatitude();
    double lon2 = end.getLongitude();

    double phi1 = lat1 * (M_PI / 180.0);
    double phi2 = lat2 * (M_PI / 180.0);
    double dphi = (lat2 - lat1) * (M_PI / 180.0);
    double dlambda = (lon2 - lon1) * (M_PI / 180.0);

    double a = std::sin(dphi / 2) * std::sin(dphi / 2) +
               std::cos(phi1) * std::cos(phi2) *
               std::sin(dlambda / 2) * std::sin(dlambda / 2);

    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = R * c;

    double factor = 1.0;
    if (highwayType == "motorway" || highwayType == "trunk") factor = 0.5;
    else if (highwayType == "primary" || highwayType == "secondary") factor = 0.75;
    else if (highwayType == "tertiary" || highwayType == "residential") factor = 1.0;
    else if (highwayType == "living_street" || highwayType == "unclassified") factor = 1.25;
    else if (highwayType == "service" || highwayType == "track") factor = 1.5;

    return distance * factor;
}

std::list<unsigned long> Graph::bfs(unsigned long startId) {
    std::list<unsigned long> result;
    std::set<unsigned long> visited;
    std::queue<unsigned long> q;

    if (vertexIndexMap.find(startId) == vertexIndexMap.end())
        return result;

    q.push(startId);
    visited.insert(startId);

    while (!q.empty()) {
        unsigned long currentId = q.front();
        q.pop();
        result.push_back(currentId);

        unsigned int index = vertexIndexMap[currentId];
        const Vertex& vertex = adjacencyList[index];

        std::vector<unsigned long> neighbors;
        for (const Edge& edge : vertex.getEdges()) {
            neighbors.push_back(edge.getEnd());
        }

        std::sort(neighbors.begin(), neighbors.end());

        for (unsigned long nid : neighbors) {
            if (visited.find(nid) == visited.end() && vertexIndexMap.find(nid) != vertexIndexMap.end()) {
                q.push(nid);
                visited.insert(nid);
            }
        }
    }
    return result;
}

std::list<unsigned long> Graph::dfs(unsigned long startId) {
    std::list<unsigned long> result;
    if (vertexIndexMap.find(startId) == vertexIndexMap.end()) {
        return result;
    }

    std::set<unsigned long> visited;
    std::stack<unsigned long> s;


    s.push(startId);
    visited.insert(startId);

    while (!s.empty()) {
        unsigned long currentId = s.top();
        s.pop();

        result.push_back(currentId);

        unsigned int index = vertexIndexMap[currentId];
        const Vertex& vertex = adjacencyList[index];

        std::vector<unsigned long> neighbors;
        for (const Edge& edge : vertex.getEdges()) {
            neighbors.push_back(edge.getEnd());
        }

        std::sort(neighbors.begin(), neighbors.end(), std::greater<unsigned long>());

        for (unsigned long neighborId : neighbors) {
            if (visited.find(neighborId) == visited.end()) {
                visited.insert(neighborId);
                s.push(neighborId);
            }
        }
    }

    return result;
}



void Graph::compact() {
    while (true) {
        unsigned long idToRemove = 0;

        for (const Vertex& v : adjacencyList) {
            if (v.getEdges().size() == 1 && v.getIncoming().size() == 1) {
                unsigned long prevId = v.getIncoming().front().getStart();
                unsigned long nextId = v.getEdges().front().getEnd();
                if (prevId != nextId) {
                    addEdge(prevId, nextId, v.getEdges().front().getHighwayType());
                    idToRemove = v.getId();
                    break;
                }
            }
            else if (v.getEdges().size() == 2 && v.getIncoming().size() == 2) {
                unsigned long start1 = v.getIncoming().front().getStart();
                unsigned long start2 = v.getIncoming().back().getStart();
                std::set<unsigned long> startSet = {start1, start2};
                
                unsigned long end1 = v.getEdges().front().getEnd();
                unsigned long end2 = v.getEdges().back().getEnd();
                std::set<unsigned long> endSet = {end1, end2};

                if (startSet == endSet) {
                    addEdge(start1, start2, v.getEdges().front().getHighwayType());
                    addEdge(start2, start1, v.getEdges().front().getHighwayType());
                    idToRemove = v.getId();
                    break;
                }
            }
        }

        if (idToRemove != 0) {
            removeVertex(idToRemove);
        } else {
            break;
        }
    }
}

std::list<unsigned long> Graph::dijkstra(unsigned long startId, unsigned long endId) {
    std::list<unsigned long> path;
    if (vertexIndexMap.find(startId) == vertexIndexMap.end() || vertexIndexMap.find(endId) == vertexIndexMap.end())
        return path;

    std::map<unsigned long, double> dist;
    std::map<unsigned long, unsigned long> prev;

    auto cmp = [](const std::pair<double, unsigned long>& a, const std::pair<double, unsigned long>& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second > b.second;
    };
    std::priority_queue<std::pair<double, unsigned long>,
                        std::vector<std::pair<double, unsigned long>>,
                        decltype(cmp)> pq(cmp);

    for (const Vertex& v : adjacencyList) {
        dist[v.getId()] = std::numeric_limits<double>::infinity();
    }
    dist[startId] = 0.0;
    pq.push({0.0, startId});

    while (!pq.empty()) {
        auto [currentDist, currentId] = pq.top();
        pq.pop();

        if (currentId == endId) break;

        unsigned int index = vertexIndexMap[currentId];
        const Vertex& vertex = adjacencyList[index];

        for (const Edge& edge : vertex.getEdges()) {
            unsigned long neighbor = edge.getEnd();
            double weight = edge.getDistance();
            double alt = currentDist + weight;
            if (alt < dist[neighbor] || (std::abs(alt - dist[neighbor]) < 1e-9 && neighbor < prev[neighbor])) {
                dist[neighbor] = alt;
                prev[neighbor] = currentId;
                pq.push({alt, neighbor});
            }
        }
    }

    for (unsigned long at = endId; ; at = prev[at]) {
        path.push_front(at);
        if (at == startId) break;
        if (prev.find(at) == prev.end()) {
            path.clear();
            break;
        }
    }
    return path;
}

void Graph::printGraph() const {
    std::cout << "Vertices:" << std::endl;
    for (const auto& vertex : adjacencyList) {
        std::cout << "ID: " << vertex.getId()
             << ", Lat: " << vertex.getLatitude()
             << ", Lon: " << vertex.getLongitude() << std::endl;
    }

    std::cout << "\nEdges:\n";
    for (const auto& vertex : adjacencyList) {
        for (const auto& edge : vertex.getEdges()) {
            std::cout << "From: " << edge.getStart()
                 << " -> To: " << edge.getEnd()
                 << ", Distance: " << edge.getDistance() << std::endl;

        }
    }
}
