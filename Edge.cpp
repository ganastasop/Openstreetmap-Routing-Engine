#include "Edge.hpp"

Edge::Edge(unsigned long Start, unsigned long End, double Distance, std::string HighwayType)
    : start(Start), end(End), distance(Distance), highwayType(HighwayType) {}

Edge::Edge(const Edge& edge)
    : start(edge.getStart()), end(edge.getEnd()), distance(edge.getDistance()), highwayType(edge.getHighwayType()) {}

void Edge::setStart(unsigned long Start) {start = Start;}
void Edge::setEnd(unsigned long End) {end = End;}
void Edge::setDistance(double Distance) {distance = Distance;}

unsigned long Edge::getStart() const {return start;}
unsigned long Edge::getEnd() const {return end;}
double Edge::getDistance() const {return distance;}
std::string Edge::getHighwayType() const {return highwayType;}