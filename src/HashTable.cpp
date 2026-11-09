#include <map>
#include <iostream>
// Purpose: Implements the custom hash table used to store and search campus facilities by their IDs. Handles collisions, duplicate IDs, and hash table statistics.
// Author: Ryan Anyanga

#include "HashTable.h"
#include <cctype>
HashTable::HashTable(std::size_t capacity)
    : capacity(capacity == 0 ? 10 : capacity),
      entryCount(0),
      collisionCount(0) {
    buckets.resize(this->capacity);
}

std::size_t HashTable::hashFunction(const std::string& id) const {
    // Simple hash function implementation - replace with a more robust one if needed
    std::size_t hash = 0;
    for (char c : id) {
        hash = (hash * 31) + static_cast<std::size_t>(c);
    }
    return hash % capacity;
}

bool HashTable::insert(const Facility& facility) {
    std::string text = facility.getId();
    for (char& c : text) {
    c = static_cast<char>(
        std::toupper(static_cast<unsigned char>(c))
    );
}
    
    // Validate the facility ID format
if (text.length() != 4 || text[0] != 'F') {
    return false;
}

// Make sure the last three characters are digits
for (std::size_t i = 1; i < text.length(); i++) {
    if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
        return false;
    }
}
std::size_t index = hashFunction(text);
    // Check if the facility ID already exists
    for (const auto& existingFacility : buckets[index]) {
        if (existingFacility.getId() == text) {
            return false; // Facility with this ID already exists
        }
    }

    // // Count a collision if the bucket is already occupied
if (!buckets[index].empty()) {
    collisionCount++;
}

// Create a facility with the normalized ID
Facility normalizedFacility(
    text,
    facility.getName(),
    facility.getBuilding(),
    facility.getCategory()
);

// Insert the facility
buckets[index].push_back(normalizedFacility);
entryCount++;
return true;
}

const Facility* HashTable::findById(const std::string& id) const {
    std::string text = id;
    for (char& c : text) {
        c = static_cast<char>(
            std::toupper(static_cast<unsigned char>(c))
        );
    }
    std::size_t index = hashFunction(text);

    for (const auto& facility : buckets[index]) {
        if (facility.getId() == text) {
            return &facility; // Return pointer to the found facility
        }
    }
    return nullptr; // Facility not found
}

std::vector<Facility> HashTable::findByName(const std::string& name) const {
    std::vector<Facility> results;

    for (const auto& bucket : buckets) {
        for (const auto& facility : bucket) {
            if (facility.getName() == name) {
                results.push_back(facility);
            }
        }
    }

    return results;
}
std::vector<Facility> HashTable::findByCategory(const std::string& category) const {
    std::vector<Facility> results;

    for (const auto& bucket : buckets) {
        for (const auto& facility : bucket) {
            if (facility.getCategory() == category) {
                results.push_back(facility);
            }
        }
    }

    return results;
}
std::vector<Facility> HashTable::findByBuilding(const std::string& building) const {
    std::vector<Facility> results;

    for (const auto& bucket : buckets) {
        for (const auto& facility : bucket) {
            if (facility.getBuilding() == building) {
                results.push_back(facility);
            }
        }
    }

    return results;
}
// Return the number of stored facilities
std::size_t HashTable::getEntryCount() const {
    return entryCount;
}

// Return the number of recorded collisions
std::size_t HashTable::getCollisionCount() const {
    return collisionCount;
}

void HashTable::printCategoryReport() const {
    std::map<std::string, int> categoryCounts;

    // Count facilities in each category
    for (const auto& bucket : buckets) {
        for (const auto& facility : bucket) {
            categoryCounts[facility.getCategory()]++;
        }
    }

    // Display the report
    std::cout << "\nFacility Category Report\n";
    std::cout << "------------------------\n";

    for (const auto& category : categoryCounts) {
        std::cout << category.first << ": "
                  << category.second << '\n';
    }

    std::cout << "Total Facilities: "
              << entryCount << '\n';
}