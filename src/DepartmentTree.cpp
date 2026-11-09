#include "DepartmentTree.h"
#include <cctype> 
#include <fstream>
#include <iostream>

using namespace std;

static string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");

    if (start == string::npos) {
        return ""; 
    }


size_t end = text.find_last_not_of(" \t\r\n");

return text.substr(start, end - start +1);
}

static string toLower(string text){
    for (char& ch: text) {
        ch = tolower(static_cast<unsigned char>(ch));
    }

    return text;
}

DepartmentTree::DepartmentTree() {
    root = nullptr;
}

DepartmentTree::~DepartmentTree() {
    deleteTree(root);
}

void DepartmentTree::deleteTree(DepartmentNode* node) {
    if (node == nullptr) {
        return;
    }

    for (DepartmentNode* child : node-> children) {
        deleteTree(child);
    }

    delete node;
}

bool DepartmentTree::isEmpty() const {
    return root == nullptr;
}

bool DepartmentTree::hasDepartment(const string& name) {
    return findDepartment(root, name) != nullptr;
}

bool DepartmentTree::addDepartment(const string& name,
                                   const string& parentName) {
    string departmentName = trim(name);
    string parent = trim(parentName);

    if (departmentName.empty()) {
        return false;
    }

    if (findDepartment(root, departmentName) != nullptr) {
        return false;
    }
           
    
    if (root == nullptr) {
        root = new DepartmentNode;
        root->name = departmentName;
        root->parent = nullptr;
        return true;
    }

    DepartmentNode* parentNode = findDepartment(root, parent);

    if (parentNode == nullptr) {
        return false;
    }

    DepartmentNode* newNode = new DepartmentNode;
    newNode->name = departmentName;
    newNode->parent = parentNode;

    parentNode->children.push_back(newNode);

    return true;
}

DepartmentNode* DepartmentTree::findDepartment(
    DepartmentNode* node, const string& name) {

    if (node == nullptr) {
        return nullptr;
    }

    string currentName = toLower(node->name);
    string searchName = toLower(trim(name));


    if (currentName == searchName) {
        return node;
    }

    for (DepartmentNode* child : node->children) {
        DepartmentNode* result = findDepartment(child, name);

        if (result != nullptr) {
            return result;
        }
    }

    return nullptr; 
}

bool DepartmentTree::loadFromFile(const string& filename) {

    ifstream file(filename);

    int accepted = 0;
    int rejected = 0;

    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << endl;
        cout << filename << ": Accepted: 0, Rejected: 0" << endl;
        return false;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {

        lineNumber++;

        string cleanedLine = trim(line);

        if (cleanedLine.empty() || cleanedLine[0] == '#') {
            continue;
        }


        size_t position = cleanedLine.find('|');

        if (position == string::npos) {
            cout << "warning: " << filename
                 << ", line " << lineNumber
                 << ": missing | separator" << endl;

            rejected++;
            continue;
        }

        if (cleanedLine.find('|', position +1) != string::npos) {
            cout << "warning: " << filename
                 << ", line" << lineNumber
                 << ": entra | separator" << endl; 
            rejected++;
            continue;
        }

        

        string departmentName = trim(cleanedLine.substr(0, position));
        string parentName = trim(cleanedLine.substr(position + 1));

        if (departmentName.empty() || parentName.empty()) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": empty department or parent name" << endl;
            rejected++;
            continue;
        }

        if (findDepartment(root, departmentName) != nullptr) {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": duplicate department name" << endl;
            rejected++;
            continue;
        }

        if (root == nullptr) {

            if (toLower(parentName) !="none") {
                cout << "Warning: " << filename
                     << ", line " << lineNumber
                     << ": root parent must be NONE" << endl;
                rejected++;
                continue;
            }

        } else {

            if (findDepartment(root, parentName) == nullptr) {
                cout << "Warning: " << filename
                     << ", line " << lineNumber
                     << ": parent department not found" << endl;
                rejected++;
                continue;
            }
        }

        if (addDepartment(departmentName, parentName)) {
            accepted++;
        } else {
            cout << "Warning: " << filename
                 << ", line " << lineNumber
                 << ": could not add department" << endl;
            rejected++;
        }
    }


    file.close();

    cout << filename << ": Accepted: " << accepted
         << ", Rejected: " << rejected << endl;

    return true;
}

void DepartmentTree::displayNode(DepartmentNode* node, int depth) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < depth * 4; i++) {
        cout << " ";
    }

    cout << node->name << endl;

    for (DepartmentNode* child : node-> children) {
        displayNode(child, depth +1);
    }
}

void DepartmentTree::displayHierarchy() {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    displayNode(root, 0);
}

void DepartmentTree::preorderNode(DepartmentNode* node) {
    if (node == nullptr) {
        return;
    }

    cout << node->name << endl;

    for (DepartmentNode* child : node->children) {
        preorderNode(child);
    }
}

void DepartmentTree::preorderTraversal() {
    if (root == nullptr) {
        cout << "Department tree is empty." << endl;
        return;
    }

    preorderNode(root);
}

void DepartmentTree::searchDepartment(const string& name) {

    DepartmentNode* department = findDepartment(root, name);

    if (department == nullptr) {
        cout << "Department not found." << endl;
        return;
    }

    cout << "Department: " << department->name << endl;
    
    if (department->parent == nullptr) {
        cout << "Parent: None" << endl;
    } else {
        cout << "Parent: " << department->parent->name<< endl;
    }

    cout << "Direct Children: "
         << department->children.size() << endl;

    int depth = 0;
    DepartmentNode* current = department; 

    while (current->parent != nullptr) {
        depth++;
        current = current->parent;
    }

    cout << "Depth: " << depth << endl;

}