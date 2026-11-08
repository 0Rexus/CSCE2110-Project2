#include <iostream>
#include "HashTable.h"

int main() {
    HashTable table(10);

    Facility f1("F001", "Library", "BLB", "Study");
    Facility f2("F002", "Computer Lab", "GAB", "Technology");
    Facility f3("f003", "Meeting Room", "Union", "Meeting");

    std::cout << "Insert F001: " << table.insert(f1) << '\n';
    std::cout << "Insert F002: " << table.insert(f2) << '\n';
    std::cout << "Insert f003: " << table.insert(f3) << '\n';

    std::cout << "Duplicate F001: " << table.insert(f1) << '\n';

    const Facility* found = table.findById("f003");

    if (found != nullptr) {
        std::cout << "Found: " << found->getName() << '\n';
    } else {
        std::cout << "Facility not found\n";
    }

    std::cout << "Entries: " << table.getEntryCount() << '\n';
    std::cout << "Collisions: " << table.getCollisionCount() << '\n';



// Test invalid facility IDs
Facility invalid1("F12A", "Invalid Room", "BLB", "Study");
Facility invalid2("F12", "Short ID", "GAB", "Study");


std::cout << "Invalid F12A: " << table.insert(invalid1) << '\n';
std::cout << "Invalid F12: " << table.insert(invalid2) << '\n';

// Test inserting an ID that normalizes to a duplicate
Facility duplicateLower("f001", "Another Library", "BLB", "Study");
std::cout << "Duplicate f001: " << table.insert(duplicateLower) << '\n';

// Test forced collisions using a one-bucket table
HashTable collisionTable(1);

Facility c1("F010", "Room A", "BLB", "Study");
Facility c2("F011", "Room B", "GAB", "Study");

collisionTable.insert(c1);
collisionTable.insert(c2);

std::cout << "Collision test entries: "
          << collisionTable.getEntryCount() << '\n';

std::cout << "Collision test collisions: "
          << collisionTable.getCollisionCount() << '\n' ;
          // Test searching by facility name
std::cout << "Name search results: "
          << table.findByName("Library").size() << '\n';

// Test searching by building
std::cout << "Building search results: "
          << table.findByBuilding("BLB").size() << '\n';

// Test searching by category
std::cout << "Category search results: "
          << table.findByCategory("Study").size() << '\n';

// Test searching for a nonexistent facility
std::cout << "Missing facility: "
          << (table.findById("F999") == nullptr) << '\n';
// Test the facility category report
table.printCategoryReport();
 return 0;

}