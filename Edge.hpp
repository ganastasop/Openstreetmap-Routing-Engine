#ifndef EDGE_HPP
#define EDGE_HPP

#include <string>

class Edge {
    private:
        unsigned long start;
        unsigned long end;
        double distance;
        std::string highwayType;
    public:
        Edge(unsigned long Start, unsigned long End, double Distance, std::string HighwayType);
        Edge(const Edge& edge);
        
        void setStart(unsigned long Start);
        void setEnd(unsigned long End);
        void setDistance(double Distance);

        unsigned long getStart() const;
        unsigned long getEnd() const;
        double getDistance() const;
        std::string getHighwayType() const;
};

#endif