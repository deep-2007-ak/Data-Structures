#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

struct Node {
    string name;
    bool isFolder;
    int size;
    Node* parent;
    vector<Node*> children;

    Node(string n, bool folder, int s, Node* p) {
        name = n;
        isFolder = folder;
        size = s;
        parent = p;
    }
};

Node* findChild(Node* folder, const string& name) {
    for (int i = 0; i < folder->children.size(); i++) {
        if (folder->children[i]->name == name) return folder->children[i];
    }
    return nullptr;
}

bool addNode(Node* folder, const string& name, bool isFolder, int size) {
    if (!folder->isFolder) return false;
    if (findChild(folder, name) != nullptr) return false;
    folder->children.push_back(new Node(name, isFolder, size, folder));
    return true;
}

void listChildren(Node* folder) {
    if (folder->children.empty()) {
        cout << "(empty)" << endl;
        return;
    }
    for (int i = 0; i < folder->children.size(); i++) {
        Node* c = folder->children[i];
        if (c->isFolder) cout << "[folder] " << c->name << endl;
        else cout << "[file]   " << c->name << "  (" << c->size << " KB)" << endl;
    }
}

void printTree(Node* node, int depth) {
    for (int i = 0; i < depth; i++) cout << "    ";
    if (node->isFolder) cout << "+ " << node->name << "/" << endl;
    else cout << "- " << node->name << " (" << node->size << " KB)" << endl;

    for (int i = 0; i < node->children.size(); i++)
        printTree(node->children[i], depth + 1);
}

int totalSize(Node* node) {
    if (!node->isFolder) return node->size;
    int sum = 0;
    for (int i = 0; i < node->children.size(); i++)
        sum += totalSize(node->children[i]);
    return sum;
}

int countNodes(Node* node) {
    int count = 1;
    for (int i = 0; i < node->children.size(); i++)
        count += countNodes(node->children[i]);
    return count;
}

int height(Node* node) {
    int best = 0;
    for (int i = 0; i < node->children.size(); i++) {
        int h = 1 + height(node->children[i]);
        if (h > best) best = h;
    }
    return best;
}

string fullPath(Node* node) {
    string path = "";
    while (node->parent != nullptr) {
        path = "/" + node->name + path;
        node = node->parent;
    }
    if (path == "") path = "/";
    return path;
}

Node* findByName(Node* node, const string& name) {
    if (node->name == name) return node;
    for (int i = 0; i < node->children.size(); i++) {
        Node* found = findByName(node->children[i], name);
        if (found != nullptr) return found;
    }
    return nullptr;
}

void deleteSubtree(Node* node) {
    for (int i = 0; i < node->children.size(); i++)
        deleteSubtree(node->children[i]);
    delete node;
}

bool removeChild(Node* folder, const string& name) {
    for (int i = 0; i < folder->children.size(); i++) {
        if (folder->children[i]->name == name) {
            deleteSubtree(folder->children[i]);
            folder->children.erase(folder->children.begin() + i);
            return true;
        }
    }
    return false;
}

void showHelp() {
    cout << "Commands:" << endl;
    cout << "  mkdir <name>          make a folder here" << endl;
    cout << "  touch <name> <size>   make a file here (size in KB)" << endl;
    cout << "  ls                    list what is in this folder" << endl;
    cout << "  cd <name>             go into a folder (cd .. = go up, cd / = root)" << endl;
    cout << "  pwd                   show the current path" << endl;
    cout << "  tree                  show the whole tree" << endl;
    cout << "  rm <name>             delete a file or folder (with everything inside)" << endl;
    cout << "  find <name>           search the whole tree" << endl;
    cout << "  size [name]           total size of this folder or a named item" << endl;
    cout << "  count                 how many items are inside this folder" << endl;
    cout << "  height                how many levels deep this folder goes" << endl;
    cout << "  help, exit" << endl;
}

int main() {
    Node* root = new Node("root", true, 0, nullptr);

    Node* current = root;
    cout << "Mini File System. Type 'help' to see commands." << endl;

    while (true) {
        cout<<endl;
        cout << "(" << fullPath(current) << ")$ ";
        string line;
        getline(cin, line);

        stringstream ss(line);
        string cmd, arg1, arg2;
        ss >> cmd >> arg1 >> arg2;

        if (cmd == "exit") {
            break;
        } else if (cmd == "help") {
            showHelp();
        } else if (cmd == "mkdir") {
            if (arg1 == "") cout << "Usage: mkdir <name>" << endl;
            else if (addNode(current, arg1, true, 0)) cout << "Folder created." << endl;
            else cout << "Could not create (name used or not a folder)." << endl;
        } else if (cmd == "touch") {
            int size = (arg2 == "") ? 1 : atoi(arg2.c_str());
            if (arg1 == "") cout << "Usage: touch <name> <size>" << endl;
            else if (addNode(current, arg1, false, size)) cout << "File created." << endl;
            else cout << "Could not create (name already used)." << endl;
        } else if (cmd == "ls") {
            listChildren(current);
        } else if (cmd == "cd") {
            if (arg1 == "..") {
                if (current->parent != nullptr) current = current->parent;
                else cout << "Already at root." << endl;
            } else if (arg1 == "/") {
                current = root;
            } else {
                Node* target = findChild(current, arg1);
                if (target == nullptr) cout << "No such folder." << endl;
                else if (!target->isFolder) cout << "That is a file, not a folder." << endl;
                else current = target;
            }
        } else if (cmd == "pwd") {
            cout << fullPath(current) << endl;
        } else if (cmd == "tree") {
            printTree(current, 0);
        } else if (cmd == "rm") {
            if (removeChild(current, arg1)) cout << "Deleted." << endl;
            else cout << "Nothing with that name here." << endl;
        } else if (cmd == "find") {
            Node* found = findByName(root, arg1);
            if (found == nullptr) cout << "Not found." << endl;
            else cout << "Found at: " << fullPath(found) << endl;
        } else if (cmd == "size") {
            Node* target = (arg1 == "") ? current : findChild(current, arg1);
            if (target == nullptr) cout << "No such item." << endl;
            else cout << "Total size: " << totalSize(target) << " KB" << endl;
        } else if (cmd == "count") {
            cout << "Items inside: " << countNodes(current) - 1 << endl;
        } else if (cmd == "height") {
            cout << "Levels below this folder: " << height(current) << endl;
        } else if (cmd != "") {
            cout << "Unknown command. Type 'help'." << endl;
        }
    }

    deleteSubtree(root);
    return 0;
}
