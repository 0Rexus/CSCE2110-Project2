// Purpose: Declares the HashTable class and its operations for inserting, searching, and managing campus facility records.
// Author: Ryan Anyanga
#pragma once
#include <string>
#include <vector>
#include "Facility.h"
#include <cstddef>

class HashTable {
private:
     std::vector<std::vector<Facility>> buckets;
     std::size_t capacity;
     std::size_t entryCount;
     std::size_t collisionCount;

     std::size_t hashFunction(const std::string& id) const;
public:
    HashTable(std::size_t capacity);
    bool insert(const Facility& facility);
const Facility* findById(const std::string& id) const;
std::vector<Facility> findByName(const std::string& name) const;
    std::vector<Facility> findByBuilding(const std::string& building) const;
    std::vector<Facility> findByCategory(const std::string& category) const;
    std::size_t getEntryCount() const;
    std::size_t getCollisionCount() const;
    void printCategoryReport() const;
};