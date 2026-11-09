#include "CampusGraph.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <cctype>
#include <iomanip>

using namespace std;

string CampusGraph::trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

string CampusGraph::toLower(const string& text) {
    string result = text;
    for (char& ch : result) {
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

CampusGraph::CampusGraph() : totalConnections(0) {}

int CampusGraph::findBuildingIndex(const string& name) const {
    string target = toLower(trim(name));
    for (size_t i = 0; i < vertices.size(); i++) {
        if (toLower(vertices[i].name) == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool CampusGraph::hasBuilding(const string& name) const {
    return findBuildingIndex(name) != -1;
}

string CampusGraph::getOriginalSpelling(const string& name) const {
    int idx = findBuildingIndex(name);
    if (idx != -1) {
        return vertices[idx].name;
    }
    return name;
}

size_t CampusGraph::getBuildingCount() const {
    return vertices.size();
}

size_t CampusGraph::getConnectionCount() const {
    return totalConnections;
}

bool CampusGraph::loadBuildings(const string& filename) {
    ifstream file(filename);
    int accepted = 0;
    int rejected = 0;
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        cout << filename << ": 0 accepted, 0 rejected" << endl;
        return false;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        string cleaned = trim(line);
        if (cleaned.empty() || cleaned[0] == '#') {
            continue;
        }

        if (hasBuilding(cleaned)) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": duplicate building name" << endl;
            rejected++;
            continue;
        }

        BuildingVertex v;
        v.name = cleaned;
        vertices.push_back(v);
        accepted++;
    }

    file.close();
    cout << filename << ": " << accepted << " accepted, " << rejected << " rejected" << endl;
    return true;
}

bool CampusGraph::loadConnections(const string& filename) {
    ifstream file(filename);
    int accepted = 0;
    int rejected = 0;
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        cout << filename << ": 0 accepted, 0 rejected" << endl;
        return false;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        string cleaned = trim(line);
        if (cleaned.empty() || cleaned[0] == '#') {
            continue;
        }

        size_t firstPipe = cleaned.find('|');
        if (firstPipe == string::npos) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": missing | separator" << endl;
            rejected++;
            continue;
        }
        if (cleaned.find('|', firstPipe + 1) != string::npos) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": extra | separator" << endl;
            rejected++;
            continue;
        }

        string buildingA = trim(cleaned.substr(0, firstPipe));
        string buildingB = trim(cleaned.substr(firstPipe + 1));

        if (buildingA.empty() || buildingB.empty()) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": empty building name" << endl;
            rejected++;
            continue;
        }

        if (toLower(buildingA) == toLower(buildingB)) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": self-connection not allowed" << endl;
            rejected++;
            continue;
        }

        int idxA = findBuildingIndex(buildingA);
        int idxB = findBuildingIndex(buildingB);

        if (idxA == -1 || idxB == -1) {
            string missing = (idxA == -1) ? buildingA : buildingB;
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": unknown building reference '" << missing << "'" << endl;
            rejected++;
            continue;
        }

        // Check if connection already exists (undirected, check idxB in idxA's neighbors)
        bool alreadyConnected = false;
        for (int neighbor : vertices[idxA].neighbors) {
            if (neighbor == idxB) {
                alreadyConnected = true;
                break;
            }
        }

        if (alreadyConnected) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": duplicate connection" << endl;
            rejected++;
            continue;
        }

        // Valid undirected edge: add to both endpoints preserving insertion order
        vertices[idxA].neighbors.push_back(idxB);
        vertices[idxB].neighbors.push_back(idxA);
        totalConnections++;
        accepted++;
    }

    file.close();
    cout << filename << ": " << accepted << " accepted, " << rejected << " rejected" << endl;
    return true;
}

void CampusGraph::displayGraph() const {
    if (vertices.empty()) {
        cout << "Campus graph has no buildings loaded." << endl;
        return;
    }

    for (const auto& v : vertices) {
        cout << v.name << ": ";
        if (v.neighbors.empty()) {
            cout << "(no connections)" << endl;
        } else {
            for (size_t i = 0; i < v.neighbors.size(); i++) {
                cout << vertices[v.neighbors[i]].name;
                if (i + 1 < v.neighbors.size()) {
                    cout << ", ";
                }
            }
            cout << endl;
        }
    }
}

void CampusGraph::displayCountsAndDegrees() const {
    if (vertices.empty()) {
        cout << "Campus graph has no buildings loaded." << endl;
        return;
    }

    cout << left << setw(25) << "Building" << "Degree" << endl;
    cout << "--------------------------------" << endl;
    for (const auto& v : vertices) {
        cout << left << setw(25) << v.name << v.neighbors.size() << endl;
    }
    cout << "--------------------------------" << endl;
    cout << "Total buildings: " << vertices.size() << endl;
    cout << "Total connections: " << totalConnections << endl;
}

void CampusGraph::runBFS(const string& startBuilding) const {
    if (vertices.empty()) {
        cout << "Campus graph has no buildings loaded." << endl;
        return;
    }

    int startIdx = findBuildingIndex(startBuilding);
    if (startIdx == -1) {
        cout << "Error: Building '" << startBuilding << "' not found in campus graph." << endl;
        return;
    }

    vector<bool> visited(vertices.size(), false);
    vector<int> distance(vertices.size(), 0);
    queue<int> q;

    // Mark visited at enqueue as required by BFS-1
    visited[startIdx] = true;
    distance[startIdx] = 0;
    q.push(startIdx);

    int reachableCount = 0;
    cout << "From " << vertices[startIdx].name << ":" << endl;

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        reachableCount++;

        cout << vertices[current].name << " (distance " << distance[current] << ")" << endl;

        for (int neighbor : vertices[current].neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true; // Mark when added to queue
                distance[neighbor] = distance[current] + 1;
                q.push(neighbor);
            }
        }
    }

    cout << "Reachable: " << reachableCount << " of " << vertices.size() << " buildings" << endl;
}