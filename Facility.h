// Purpose: Declares the Facility class, which stores campus facility information, including the facility ID, name, building, and category. Provides methods to access this information.
// Author: Ryan Anyanga
#pragma once
#include <string>

class Facility {
private:
    std::string id;
    std::string name;
    std::string building;
    std::string category;

public:
    Facility(const std::string& id, const std::string& name, const std::string& building, const std::string& category)
        : id(id), name(name), building(building), category(category) {}

    std::string getId() const {
        return id;
    }

    std::string getName() const {
        return name;
    }

    std::string getBuilding() const {
        return building;
    }

    std::string getCategory() const {
        return category;
    }
};