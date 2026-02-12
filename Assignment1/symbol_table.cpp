#include <iostream>
#include <fstream>  // Required for file input/output
#include <string>
#include <sstream>


using namespace std;

// -----------------------------------------------------------------
// 1. SymbolInfo Class
// -----------------------------------------------------------------
class SymbolInfo {
private:
    string name;
    string type;
    SymbolInfo* next;

//Write your code here (constructor, setter , getter if required)
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

    unsigned long sdbmhash(const string& str) {
        unsigned long hash = 0;
        for (char c : str) {
            hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }

    int hash(const string& name) {
        return sdbmhash(name) % total_buckets;
    }

public:
    //Implement the constructor, destructor and other functions
};

// -----------------------------------------------------------------
// 3. SymbolTable Class
// -----------------------------------------------------------------
class SymbolTable {
private:
    ScopeTable* current;
    int total_buckets;

public:
    //Implement constructor, destructor and other functions
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
    } catch (const std::exception& e) {
        cerr << "Error: Invalid bucket size on first line." << endl;
        return 1;
    }

    // Create the SymbolTable instance here


    // Process the rest of the file
    while (getline(inputFile, line)) {
        // print the command to the console/file

        stringstream ss(line);
        char command;
        ss >> command;

        switch (command) {
            case 'I': {
                string name, type;
                ss >> name >> type;
                //Call Insert function here
                break;
            }
            case 'L': {
                string name;
                ss >> name;
                //Lookup function call
                break;
            }
            case 'D': {
                string name;
                ss >> name;
                //Delete/Remove function call
                break;
            }
            case 'P': {
                char sub_command;
                ss >> sub_command;
                if (sub_command == 'A') {
                    //printAll function call
                } else if (sub_command == 'C') {
                    //printCurrent call
                }
                break;
            }
            case 'S': {
                //Enter Scope Call
                break;
            }
            case 'E': {
                //Exit Scope call
                break;
            }
            default:
                cout << "\nInvalid command.\n";
        }
    }

    // Close the files
    inputFile.close();

    return 0;
}
