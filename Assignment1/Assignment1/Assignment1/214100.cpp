// symbol_table_solution.cpp
#include <iostream>
#include <fstream>  // Required for file input/output
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// -----------------------------------------------------------------
// 1. SymbolInfo Class
// -----------------------------------------------------------------
class SymbolInfo {
private:
    string name;
    string type;
    SymbolInfo* next;
public:
    SymbolInfo(const string& n = "", const string& t = "", SymbolInfo* nxt = NULL)
        {
            this->name = n;
            this->type = t;
            this->next = nxt;
        }
    const string& getName() 
    { 
        return name; 
    }
    const string& getType()
    { 
        return type; 
    }
    void setType(const string& t) 
    { 
        type = t; 
    }

    SymbolInfo* getNext() 
    { 
        return next; 
    }
    void setNext(SymbolInfo* nxt) 
    { 
        next = nxt; 
    }
};

// -----------------------------------------------------------------
// 2. ScopeTable Class
// -----------------------------------------------------------------
class ScopeTable {
private:
    SymbolInfo** table;
    int total_buckets;
    ScopeTable* parentScope;
    string id;
    int children_count;

    unsigned long sdbmhash(const string& str) const {
        unsigned long hash = 0;
        for (char c : str) {
            hash = (unsigned long)(unsigned char)c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    int hash(const string& name) const {
        return (int)(sdbmhash(name) % (unsigned long)total_buckets);
    }

public:
    ScopeTable(int n = 1, ScopeTable* parent = NULL, const string& myid = "")
    {
        this->total_buckets = n;
        this->parentScope = parent;
        this->id = myid;
        this->children_count = 0;
        table = new SymbolInfo*[total_buckets];
        for (int i = 0; i < total_buckets; ++i){
            table[i] = NULL;
        }
    }

    ~ScopeTable() {
        for (int i = 0; i < total_buckets; ++i) {
            SymbolInfo* cur = table[i];
            while (cur) {
                SymbolInfo* tmp = cur->getNext();
                delete cur;
                cur = tmp;
            }
            table[i] = NULL;
        }
        delete[] table;
    } 

    pair<bool, pair<int,int>> insert(const string& name, const string& type) {
        int idx = hash(name);
        SymbolInfo* cur = table[idx];
        int pos = 0;
        while (cur) {
            if (cur->getName() == name) {
                return {false, {-1,-1}}; 
            }
            cur = cur->getNext();
            ++pos;
        }
        SymbolInfo* node = new SymbolInfo(name, type, table[idx]);
        table[idx] = node;
        return {true, {idx, 0}};
    }
    pair<SymbolInfo*, pair<int,int>> lookupLocal(const string& name) {
        int idx = hash(name);
        SymbolInfo* cur = table[idx];
        int pos = 0;
        while (cur) {
            if (cur->getName() == name) {
                return {cur, {idx, pos}};
            }
            cur = cur->getNext();
            ++pos;
        }
        return {NULL, {-1,-1}};
    }

    pair<bool, pair<int,int>> remove(const string& name) {
        int idx = hash(name);
        SymbolInfo* cur = table[idx];
        SymbolInfo* prev = NULL;
        int pos = 0;
        while (cur) {
            if (cur->getName() == name) {
                if (prev) prev->setNext(cur->getNext());
                else table[idx] = cur->getNext();
                delete cur;
                return {true, {idx, pos}};
            }  
            prev = cur;
            cur = cur->getNext();
            ++pos;
        }
        return {false, {-1,-1}};
    }

    void print(){
        cout << "ScopeTable # " << id << '\n';
        for (int i = 0; i < total_buckets; ++i) {
            cout << i << " -->";
            SymbolInfo* cur = table[i];
           
            while (cur) {
                cout << "< " << cur->getName() << " : " << cur->getType() << ">";
                
                cur = cur->getNext();
            }
            cout << '\n';
        }
    }

    ScopeTable* getParent() 
    { 
        return parentScope; 
    }
    const string& getId()
    { 
        return id; 
    }
    int nextChildSerial() 
    { 
        return ++children_count; 
    }
};

// -----------------------------------------------------------------
// 3. SymbolTable Class
// -----------------------------------------------------------------
class SymbolTable {
private:
    ScopeTable* current;
    int total_buckets;

public:
    SymbolTable(int buckets = 1){
        this->total_buckets = buckets;
        current = new ScopeTable(total_buckets, NULL, "1");
    }

    ~SymbolTable() {
        while (current) {
            ScopeTable* parent = current->getParent();
            delete current;
            current = parent;
        }
    }

    void enterScope() {
        if (!current) {
            current = new ScopeTable(total_buckets, NULL, "1");
            cout << "New ScopeTable with id " << current->getId() << " created\n";
            return;
        }
        int serial = current->nextChildSerial();
        string newId = current->getId() + "." + to_string(serial);
        ScopeTable* child = new ScopeTable(total_buckets, current, newId);
        current = child;
        cout << "New ScopeTable with id " << current->getId() << " created\n";
    }

    void exitScope() {
        if (!current) return;
        cout << "ScopeTable with id " << current->getId() << " removed\n";
        ScopeTable* parent = current->getParent();
        delete current;
        current = parent;
    }

    bool insert(const string& name, const string& type) {
        if (!current) return false;
        auto res = current->insert(name, type);
        if (res.first) {
            cout << "Inserted in ScopeTable# " << current->getId()
                 << " at position " << res.second.first << ", " << res.second.second << '\n';
            return true;
        } else {
            cout << "<<" << name << "," << type << "> already exists in current ScopeTable\n";
            return false;
        }
    }

    bool remove(const string& name) {
        if (!current) return false;
        
        auto found = current->lookupLocal(name);
        if (found.first) {
            cout << "Found in ScopeTable# " << current->getId()
                 << " at position " << found.second.first << ", " << found.second.second << '\n';

            auto res = current->remove(name);
            cout << "Deleted Entry " << res.second.first << ", " << res.second.second
                 << " from current ScopeTable\n";
            return true;
        } else {
            cout << "Not found\n";
            cout << name << " not found\n";
            return false;
        }
    }
    
    SymbolInfo* lookup(const string& name) {
        ScopeTable* st = current;
        while (st) {
            auto res = st->lookupLocal(name);
            if (res.first) {
                cout << "Found in ScopeTable#  " << st->getId()
                     << " at position " << res.second.first << ", " << res.second.second << '\n';
                return res.first;
            }
            st = st->getParent();
        }
        cout << "Not found\n";
        return NULL;
    }

    void printCurrentScope() const {
        if (!current) return;
        current->print();
    }

    void printAllScope() const {
       
        ScopeTable* st = current;
        while (st) {
            st->print();
            st = st->getParent();
        }
    }
};

// -----------------------------------------------------------------
// 4. Main Function
// -----------------------------------------------------------------
int main() {
    // Open input file
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt" << endl;
        return 1;
    }

    string line;
    int total_buckets;

    // Read the first line for total buckets
    if (!getline(inputFile, line)) {
        cerr << "Error: Input file is empty." << endl;
        return 1;
    }

    try {
        total_buckets = stoi(line);
        if (total_buckets <= 0) throw runtime_error("invalid bucket count");
    } catch (const std::exception& e) {
        cerr << "Error: Invalid bucket size  on first line." << endl;
        return 1;
    }

    // Create the SymbolTable instance here
    SymbolTable mySymbolTable(total_buckets);

    // Process the rest of the file
    while (getline(inputFile, line)) {
        if (line.size() == 0) continue; // skip blank lines

        // Echo the command line exactly like sample output, then a blank line
        cout << line << "\n\n";

        stringstream ss(line);
        char command;
        ss >> command;

        switch (command) { 
            case 'I': {
                string name, type;
                ss >> name >> type;
                if (name.size() && type.size()) {
                    mySymbolTable.insert(name, type);
                }
                else cout << "Invalid I command format\n";
                break;
            }
            case 'L': {
                string name;
                ss >> name;
                if (name.size()) mySymbolTable.lookup(name);
                else cout << "Invalid L command format\n";
                break;
            }
            case 'D': {
                string name;
                ss >> name;
                if (name.size()) mySymbolTable.remove(name);
                else cout << "Invalid D command format\n";
                break;
            }
            case 'P': {
                char sub_command;
                ss >> sub_command;
                if (sub_command == 'A') {
                    mySymbolTable.printAllScope();
                } else if (sub_command == 'C') {
                    mySymbolTable.printCurrentScope();
                } else {
                    cout << "Invalid P sub-command\n";
                }
                break;
            }
            case 'S': {
                mySymbolTable.enterScope();
                break;
            }
            case 'E': {
                mySymbolTable.exitScope();
                break;
            }
            default:
                cout << "Invalid command: " << command << '\n';
        }
        cout << '\n';
    }

    // Close the files
    inputFile.close();

    return 0;
}
