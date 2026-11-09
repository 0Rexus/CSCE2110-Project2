#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

#include "Facility.h"
#include "HashTable.h"
#include "CampusGraph.h"
#include "DepartmentTree.h"

using namespace std;

static string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

static string toUpper(string text) {
    for (char& ch : text) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return text;
}

static bool isValidFacilityId(const string& id) {
    if (id.length() != 4 || id[0] != 'F') {
        return false;
    }
    for (size_t i = 1; i < id.length(); i++) {
        if (!isdigit(static_cast<unsigned char>(id[i]))) {
            return false;
        }
    }
    return true;
}

static bool loadFacilitiesFile(const string& filename,
                               const CampusGraph& graph,
                               HashTable& table,
                               vector<Facility>& acceptedList) {
    ifstream file(filename);
    int accepted = 0;
    int rejected = 0;

    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        cout << filename << ": 0 accepted, 0 rejected (hash capacity "
             << table.getCapacity() << ", entries " << table.getEntryCount()
             << ", collisions " << table.getCollisionCount() << ")" << endl;
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

        // Count pipe separators
        vector<string> fields;
        stringstream ss(cleaned);
        string item;
        while (getline(ss, item, '|')) {
            fields.push_back(trim(item));
        }
        if (!cleaned.empty() && cleaned.back() == '|') {
            fields.push_back("");
        }

        if (fields.size() != 4) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": invalid field count (expected 4)" << endl;
            rejected++;
            continue;
        }

        string rawId = fields[0];
        string name = fields[1];
        string building = fields[2];
        string category = fields[3];

        string normId = toUpper(rawId);

        if (!isValidFacilityId(normId)) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": invalid facility ID format '" << rawId << "'" << endl;
            rejected++;
            continue;
        }

        if (name.empty()) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": empty facility name" << endl;
            rejected++;
            continue;
        }

        if (category.empty()) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": empty facility category" << endl;
            rejected++;
            continue;
        }

        if (!graph.hasBuilding(building)) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": unknown building reference '" << building << "'" << endl;
            rejected++;
            continue;
        }

        // Use original building spelling from graph
        string buildingSpelling = graph.getOriginalSpelling(building);

        // Check duplicate ID in hash table
        if (table.findById(normId) != nullptr) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": duplicate facility ID '" << normId << "'" << endl;
            rejected++;
            continue;
        }

        Facility fac(normId, name, buildingSpelling, category);
        if (table.insert(fac)) {
            acceptedList.push_back(fac);
            accepted++;
        } else {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": failed to insert facility" << endl;
            rejected++;
        }
    }

    file.close();
    cout << filename << ": " << accepted << " accepted, " << rejected
         << " rejected (hash capacity " << table.getCapacity()
         << ", entries " << table.getEntryCount()
         << ", collisions " << table.getCollisionCount() << ")" << endl;
    return true;
}

static void printMenu() {
    cout << "\n========================================\n";
    cout << " Smart Campus Navigation System (M1)\n";
    cout << "========================================\n";
    cout << "1. View all facilities\n";
    cout << "2. Search facility by ID\n";
    cout << "3. View campus graph\n";
    cout << "4. Display graph counts and degrees\n";
    cout << "5. Find reachable buildings (BFS)\n";
    cout << "6. Add department\n";
    cout << "7. Search department\n";
    cout << "8. Display department hierarchy\n";
    cout << "9. Run preorder traversal\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    string dataDir = "data";
    if (argc > 1) {
        dataDir = argv[1];
        // Strip trailing slash if present
        while (dataDir.length() > 1 && (dataDir.back() == '/' || dataDir.back() == '\\')) {
            dataDir.pop_back();
        }
    }

    CampusGraph campusGraph;
    HashTable facilityTable(10);
    vector<Facility> acceptedFacilities;
    DepartmentTree deptTree;

    // Load files in required order (LOAD-1)
    string buildingsFile = dataDir + "/buildings.txt";
    string connectionsFile = dataDir + "/connections.txt";
    string facilitiesFile = dataDir + "/facilities.txt";
    string departmentsFile = dataDir + "/departments.txt";

    campusGraph.loadBuildings(buildingsFile);
    campusGraph.loadConnections(connectionsFile);
    loadFacilitiesFile(facilitiesFile, campusGraph, facilityTable, acceptedFacilities);
    deptTree.loadFromFile(departmentsFile);

    // Interactive menu loop
    string inputLine;
    while (true) {
        printMenu();
        if (!getline(cin, inputLine)) {
            break; // EOF reached
        }

        string choiceStr = trim(inputLine);
        if (choiceStr.empty()) {
            cout << "Invalid menu choice. Please enter a valid number.\n";
            continue;
        }

        if (choiceStr.length() != 1 || !isdigit(static_cast<unsigned char>(choiceStr[0]))) {
            cout << "Invalid menu choice. Please enter a number between 0 and 9.\n";
            continue;
        }

        int choice = choiceStr[0] - '0';

        if (choice == 0) {
            cout << "Exiting program. Goodbye!\n";
            break;
        } else if (choice == 1) {
            // View all facilities (FAC-2)
            if (acceptedFacilities.empty()) {
                cout << "\nNo facilities loaded.\n";
            } else {
                cout << "\nAll Facilities:\n";
                for (const auto& f : acceptedFacilities) {
                    cout << f.getId() << " | " << f.getName() << " | "
                         << f.getBuilding() << " | " << f.getCategory() << "\n";
                }
                cout << "Total facilities: " << acceptedFacilities.size() << "\n";
            }
        } else if (choice == 2) {
            // Search facility by ID (FAC-3)
            cout << "Enter facility ID: ";
            string idInput;
            if (!getline(cin, idInput)) {
                break;
            }
            string rawId = trim(idInput);
            if (rawId.empty()) {
                cout << "Error: Facility ID cannot be blank.\n";
                continue;
            }
            string normId = toUpper(rawId);
            if (!isValidFacilityId(normId)) {
                cout << "Invalid format: an ID is F followed by exactly three digits\n";
            } else {
                const Facility* found = facilityTable.findById(normId);
                if (found != nullptr) {
                    cout << "Found: " << found->getName() << " | "
                         << found->getBuilding() << " | "
                         << found->getCategory() << "\n";
                } else {
                    cout << "Not found: no facility with ID " << normId << "\n";
                }
            }
        } else if (choice == 3) {
            // View campus graph (GRAPH-3)
            cout << "\nCampus Graph:\n";
            campusGraph.displayGraph();
        } else if (choice == 4) {
            // Display graph counts and degrees (GRAPH-4)
            cout << "\nCampus Graph Counts and Degrees:\n";
            campusGraph.displayCountsAndDegrees();
        } else if (choice == 5) {
            // Find reachable buildings (BFS-1, BFS-2, BFS-3)
            cout << "Enter starting building: ";
            string bInput;
            if (!getline(cin, bInput)) {
                break;
            }
            string startBuilding = trim(bInput);
            if (startBuilding.empty()) {
                cout << "Error: Building name cannot be blank.\n";
                continue;
            }
            cout << "\n";
            campusGraph.runBFS(startBuilding);
        } else if (choice == 6) {
            // Add department (TREE-3)
            if (deptTree.isEmpty()) {
                cout << "Tree is empty. Creating root department.\n";
                cout << "Enter root department name: ";
                string nameInput;
                if (!getline(cin, nameInput)) {
                    break;
                }
                string deptName = trim(nameInput);
                if (deptName.empty()) {
                    cout << "Error: Department name cannot be blank.\n";
                    continue;
                }
                if (deptTree.addDepartment(deptName, "NONE")) {
                    cout << "Root department '" << deptName << "' created successfully.\n";
                } else {
                    cout << "Error: Failed to create root department.\n";
                }
            } else {
                cout << "Enter department name: ";
                string nameInput;
                if (!getline(cin, nameInput)) {
                    break;
                }
                string deptName = trim(nameInput);
                if (deptName.empty()) {
                    cout << "Error: Department name cannot be blank.\n";
                    continue;
                }
                if (deptTree.hasDepartment(deptName)) {
                    cout << "Error: Department '" << deptName << "' already exists.\n";
                    continue;
                }

                cout << "Enter parent department name: ";
                string parentInput;
                if (!getline(cin, parentInput)) {
                    break;
                }
                string parentName = trim(parentInput);
                if (parentName.empty()) {
                    cout << "Error: Parent department name cannot be blank.\n";
                    continue;
                }
                if (!deptTree.hasDepartment(parentName)) {
                    cout << "Error: Parent department '" << parentName << "' not found.\n";
                    continue;
                }

                if (deptTree.addDepartment(deptName, parentName)) {
                    cout << "Department '" << deptName << "' added successfully under '" << parentName << "'.\n";
                } else {
                    cout << "Error: Failed to add department.\n";
                }
            }
        } else if (choice == 7) {
            // Search department (TREE-4)
            cout << "Enter department name to search: ";
            string nameInput;
            if (!getline(cin, nameInput)) {
                break;
            }
            string deptName = trim(nameInput);
            if (deptName.empty()) {
                cout << "Error: Department name cannot be blank.\n";
                continue;
            }
            cout << "\n";
            deptTree.searchDepartment(deptName);
        } else if (choice == 8) {
            // Display department hierarchy (TREE-5)
            cout << "\nDepartment Hierarchy:\n";
            deptTree.displayHierarchy();
        } else if (choice == 9) {
            // Run preorder traversal (TREE-6)
            cout << "\nPreorder Traversal:\n";
            deptTree.preorderTraversal();
        }
    }

    return 0;
}