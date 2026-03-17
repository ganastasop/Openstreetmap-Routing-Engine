#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "Edge.hpp"
#include <list>

class Vertex {
    private:
        unsigned long id;
        double latitude;
        double longitude;
        std::list<Edge> edges;
        std::list<Edge> incomingEdges;

    public:
        Vertex(unsigned long ID, double Latitude, double Longitude);
        Vertex(const Vertex& vertex);

        void addEdge(const Edge& edge);
        void addIncoming(const Edge& edge);


        double getLatitude() const;
        double getLongitude() const;
        unsigned long getId() const;
        std::list<Edge>& getIncoming();
        const std::list<Edge>& getIncoming() const;

        std::list<Edge>& getEdges();
        const std::list<Edge>& getEdges() const;
};

#endif
