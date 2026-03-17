#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "Vertex.hpp"
#include <vector>
#include <unordered_map>
#include <string>

class Graph {
    private:
        std::vector<Vertex> adjacencyList;
        std::unordered_map<unsigned long, unsigned int> vertexIndexMap;
        unsigned int numVertices;
    public:
        Graph(const std::string& filename);

        void addVertex(unsigned long ID, double lat, double lon);
        void removeVertex(unsigned long ID);
        void addEdge(unsigned long start, unsigned long end, std::string highwayType);
        void removeIsolatedNodes();

        const std::vector<Vertex>& getVertices() const;
        double haversine(const Vertex& v1, const Vertex& v2, const std::string& highwayType) const;

        std::list<unsigned long> bfs(unsigned long startId);
        std::list<unsigned long> dfs(unsigned long startId);

        void compact();

        std::list<unsigned long> dijkstra(unsigned long startId, unsigned long endId);

        void printGraph() const;
};

#endif
