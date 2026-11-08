#ifndef DEPARTMENTTREE_H 
#define DEPARTMENTTREE_H

#include <string>
#include <vector>

struct DepartmentNode {
    std::string name;
    DepartmentNode* parent;
    std::vector<DepartmentNode*> children;
};

class DepartmentTree {
private:
    DepartmentNode* root;

    void deleteTree(DepartmentNode* node);

    DepartmentNode* findDepartment(
        DepartmentNode* node,
        const std::string& name);

    void displayNode(DepartmentNode* node, int depth);

    void preorderNode(DepartmentNode* node);

public:
    DepartmentTree();
    ~DepartmentTree(); 


    bool addDepartment(const std::string& name,
                       const std::string& parentName);

    bool loadFromFile(const std::string& filename);

    void displayHierarchy();
    void preorderTraversal();

    void searchDepartment(const std::string& name);

};

#endif

