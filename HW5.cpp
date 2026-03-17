#include "Vertex.hpp"
#include "Edge.hpp"
#include "Graph.hpp"
#include "tinyxml2.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <iomanip>

int main() {
    Graph* graph = nullptr;
    std::string input;

    while (true) {
        std::cout << "\n";
        std::cout << "-i <filepath>  :  Import Graph from <filepath>\n";
        std::cout << "-c             :  Compact Graph\n";
        std::cout << "-p <sid> <eid> :  Estimate the shortest path between start \n";
        std::cout << "                 node with <sid> and end node with <eid>\n";
        std::cout << "-b <sid>       :  Print bfs starting from node with <sid>\n";
        std::cout << "-d <sid>       :  Print dfs starting from node with <sid>\n";
        std::cout << "-q              :  Exit without memory leaks\n";
        std::cout << "\n";
        std::cout << "\nEnter your choice: " << std::endl;

        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::string word;
        std::vector<std::string> words;

        while (iss >> word) {
            words.push_back(word);
        }


        if (words[0] == "-i") {
            delete graph;
            graph = new Graph(words[1]);
            std::cout << "Vertices loaded: " << graph->getVertices().size() << std::endl;
        }

        else if (words[0] == "-b") {
            unsigned long startId = std::stoul(words[1]);
            std::list<unsigned long> bfsResult = graph->bfs(startId);
            for (unsigned long id : bfsResult) {
                std::cout << id << "\n";
            }
        }

        else if (words[0] == "-d") {
            unsigned long startId = std::stoul(words[1]);
            std::list<unsigned long> dfsResult = graph->dfs(startId);
            for (unsigned long id : dfsResult) {
                std::cout << id << "\n";
            }
        }

        else if (words[0] == "-c") {
            graph->compact();
            std::cout << "Compact OK" << std::endl;
        }

        else if (words[0] == "-p") {
            unsigned long startId = std::stoul(words[1]);
            unsigned long endId = std::stoul(words[2]);
    
            std::list<unsigned long> path = graph->dijkstra(startId, endId);

            double totalDist = 0.0;
            auto it = path.begin();
            unsigned long prev = *it;
            ++it;

            std::ostringstream url;
            url << "https://www.google.com/maps/dir/";
            const std::vector<Vertex>& vertices = graph->getVertices();

            std::map<unsigned long, const Vertex*> idToVertex;
            for (const Vertex& v : vertices) idToVertex[v.getId()] = &v;
            for (; it != path.end(); ++it) {
                unsigned long curr = *it;
                const Vertex* v = idToVertex[prev];
                double edgeDist = 0.0;
                for (const Edge& e : v->getEdges()) {
                    if (e.getEnd() == curr) {
                        edgeDist = e.getDistance();
                        break;
                    }
                }
                totalDist += edgeDist;
                std::cout << "[" << prev << " -> " << curr << "] ";
                std::cout << std::fixed << std::setprecision(3) << totalDist << std::endl;
                prev = curr;
            }

            for (unsigned long id : path) {
                const Vertex* v = idToVertex[id];
                url << v->getLatitude() << "," << v->getLongitude() << "/";
            }
            std::cout << "\n" << url.str() << std::endl;
        }

        else if (words[0] == "-q") {
            delete graph;
            return 0;
        }
    }
    return 0;
}