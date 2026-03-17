#include "Vertex.hpp"

Vertex::Vertex(unsigned long ID, double Latitude, double Longitude) 
    :id(ID), latitude(Latitude), longitude(Longitude) {}

Vertex::Vertex(const Vertex& vertex) 
    : id(vertex.id), latitude(vertex.latitude), longitude(vertex.longitude) {}

void Vertex::addEdge(const Edge& edge) {
    edges.push_back(edge);
}

void Vertex::addIncoming(const Edge& edge) {
    incomingEdges.push_back(edge);
}



double Vertex::getLatitude() const {return latitude;}
double Vertex::getLongitude() const {return longitude;}
unsigned long Vertex::getId() const {return id;}
std::list<Edge>& Vertex::getIncoming() {return incomingEdges;}
const std::list<Edge>& Vertex::getIncoming() const {return incomingEdges;}

std::list<Edge>& Vertex::getEdges() {return edges;}
const std::list<Edge>& Vertex::getEdges() const {return edges;}
