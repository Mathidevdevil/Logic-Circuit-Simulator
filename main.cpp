#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>

using namespace std;

// Base class representing a generic logic gate
class LogicGate {
protected:
    string name;
public:
    LogicGate(string n) : name(n) {}
    virtual ~LogicGate() = default;

    string getName() const { return name; }

    // Pure virtual function to evaluate the logic operation
    virtual int evaluate(int a, int b) const = 0;

    // Print the truth table for this logic gate
    virtual void printTruthTable() const {
        cout << "\nTruth Table (" << name << "):\n";
        if (name == "NOT") {
            cout << "A | Output\n";
            cout << "0 | " << evaluate(0, 0) << "\n";
            cout << "1 | " << evaluate(1, 0) << "\n";
        } else {
            cout << "A B | Output\n";
            for (int a = 0; a <= 1; ++a) {
                for (int b = 0; b <= 1; ++b) {
                    cout << a << " " << b << " | " << evaluate(a, b) << "\n";
                }
            }
        }
        cout << "\n";
    }
};

// Derived classes for each specific logic gate
class AndGate : public LogicGate {
public:
    AndGate() : LogicGate("AND") {}
    int evaluate(int a, int b) const override { return a & b; }
};

class OrGate : public LogicGate {
public:
    OrGate() : LogicGate("OR") {}
    int evaluate(int a, int b) const override { return a | b; }
};

class NotGate : public LogicGate {
public:
    NotGate() : LogicGate("NOT") {}
    // The NOT gate only uses the first operand
    int evaluate(int a, int b) const override { return !a; }
};

class XorGate : public LogicGate {
public:
    XorGate() : LogicGate("XOR") {}
    int evaluate(int a, int b) const override { return a ^ b; }
};

class NandGate : public LogicGate {
public:
    NandGate() : LogicGate("NAND") {}
    int evaluate(int a, int b) const override { return !(a & b); }
};

class NorGate : public LogicGate {
public:
    NorGate() : LogicGate("NOR") {}
    int evaluate(int a, int b) const override { return !(a | b); }
};

// Program class summarizing all behaviors
class LogicGateSimulator {
private:
    vector<LogicGate*> gates;

    LogicGate* getGate(const string& opName) {
        for (auto gate : gates) {
            if (gate->getName() == opName) {
                return gate;
            }
        }
        return nullptr;
    }

    void toUpper(string& str) {
        for(char& c : str) c = toupper(c);
    }

    // Helper to read inputs cleanly, ignoring extra spaces and '=' signs
    string readInputStr() {
        string line;
        while (getline(cin, line)) {
            // Trim leading/trailing whitespace
            size_t first = line.find_first_not_of(" \t\r\n");
            if (string::npos == first) continue; // skip empty lines
            line = line.substr(first, (line.find_last_not_of(" \t\r\n") - first + 1));
            
            // If the line contains '=', grab everything after it
            size_t eq = line.find('=');
            if (eq != string::npos) {
                string val = line.substr(eq + 1);
                first = val.find_first_not_of(" \t\r\n");
                if (first == string::npos) continue; // nothing after =
                return val.substr(first, (val.find_last_not_of(" \t\r\n") - first + 1));
            }
            return line;
        }
        return "";
    }

public:
    LogicGateSimulator() {
        gates.push_back(new AndGate());
        gates.push_back(new OrGate());
        gates.push_back(new NotGate());
        gates.push_back(new XorGate());
        gates.push_back(new NandGate());
        gates.push_back(new NorGate());
    }

    ~LogicGateSimulator() {
        for (auto gate : gates) {
            delete gate;
        }
    }

    void run() {
        string choiceStr;
        int choice;
        int previousResult = -1; // -1 indicates no previous result
        
        while (true) {
            cout << "=== Logic Gate Simulator ===\n";
            cout << "1. Perform operation\n";
            cout << "2. Show truth table\n";
            cout << "3. Exit\n";
            cout << "Enter choice: ";
            
            choiceStr = readInputStr();
            try {
                choice = stoi(choiceStr);
            } catch (...) {
                cout << "Invalid choice!\n\n";
                continue;
            }

            if (choice == 1) {
                performOperation(previousResult);
            } else if (choice == 2) {
                showTruthTable();
            } else if (choice == 3) {
                cout << "Exiting...\n";
                break;
            } else {
                cout << "Invalid choice!\n\n";
            }
        }
    }

private:
    void performOperation(int& previousResult) {
        int a = -1, b = -1;
        string inputStr;
        
        cout << "\nA (0, 1, or PREV) = ";
        inputStr = readInputStr();
        toUpper(inputStr);
        if (inputStr == "PREV" && previousResult != -1) {
            a = previousResult;
            cout << "  (Using previous result: " << a << ")\n";
        } else {
            try {
                a = stoi(inputStr);
                if (a != 0 && a != 1) throw invalid_argument("");
            } catch (...) {
                cout << "Invalid input for A. Must be 0 or 1.\n\n"; return;
            }
        }

        cout << "B (0, 1, or PREV) = ";
        inputStr = readInputStr();
        toUpper(inputStr);
        if (inputStr == "PREV" && previousResult != -1) {
            b = previousResult;
            cout << "  (Using previous result: " << b << ")\n";
        } else {
            try {
                b = stoi(inputStr);
                if (b != 0 && b != 1) throw invalid_argument("");
            } catch (...) {
                cout << "Invalid input for B. Must be 0 or 1.\n\n"; return;
            }
        }

        string opName;
        cout << "Operation = ";
        opName = readInputStr();
        toUpper(opName);

        LogicGate* gate = getGate(opName);
        if (!gate) {
            cout << "Invalid operation!\n\n"; return;
        }

        int result = gate->evaluate(a, b);
        previousResult = result;
        
        // Output final result summary
        if (gate->getName() == "NOT") {
            cout << "\nInput: A=" << a << ", Operation=" << gate->getName() << "\n";
        } else {
            cout << "\nInput: A=" << a << ", B=" << b << ", Operation=" << gate->getName() << "\n";
        }
        cout << "Output: Result = " << result << "\n";
        
        gate->printTruthTable();
    }

    void showTruthTable() {
        string opName;
        cout << "\nEnter operation (AND, OR, NOT, XOR, NAND, NOR): ";
        opName = readInputStr();
        toUpper(opName);

        LogicGate* gate = getGate(opName);
        if (!gate) {
            cout << "Invalid operation!\n\n"; return;
        }
        gate->printTruthTable();
    }
};

int main() {
    LogicGateSimulator sim;
    sim.run();
    return 0;
}
