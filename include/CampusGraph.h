#ifndef CAMPUSGRAPH_H
#define CAMPUSGRAPH_H

#include <string>
#include <vector>
#include <cstddef>

using namespace std;

struct BuildingVertex {
    string name;
    vector<int> neighbors;
};

class CampusGraph {
private:
    vector<BuildingVertex> vertices;
    size_t totalConnections;

    static string trim(const string& text);
    static string toLower(const string& text);

public:
    CampusGraph();

    bool loadBuildings(const string& filename);
    bool loadConnections(const string& filename);

    void displayGraph() const;
    void displayCountsAndDegrees() const;
    void runBFS(const string& startBuilding) const;

    int findBuildingIndex(const string& name) const;
    bool hasBuilding(const string& name) const;
    string getOriginalSpelling(const string& name) const;
    size_t getBuildingCount() const;
    size_t getConnectionCount() const;
};

#endif