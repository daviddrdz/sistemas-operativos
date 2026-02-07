#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>

#include "Job.h"

enum { ADD, SUB, MULT, DIV, POW };

using namespace std;

class Simulator {
   private:
    bool isValidOperation(string operation) {
        string operators = "+-*/^";
        size_t operatorIndex = operation.find_first_of(operators);
        if (operatorIndex != string::npos) {
            int operand = stoi(operation.substr(operatorIndex + 1));
            if (operation[operatorIndex] != operators[DIV] && operand != 0) {
                return true;
            }
        }
        return false;
    }

    bool isValidID(int id) { return true; }

   public:
    void captureJobs(int numJobs) {
        string name, operation;
        int estimatedTime, id;
        for (int i = 0; i < numJobs; i++) {
            cout << "Nombre: ";
            getline(cin, name);
            do {
                cout << "Operation: ";
                getline(cin, operation);
            } while (!isValidOperation(operation));
            do {
                cout << "Tiempo estimado: ";
                cin >> estimatedTime;
            } while (estimatedTime < 0);
            do {
                cout << "ID: ";
                cin >> id;
            } while (!isValidID(id));
        }
    }
};

#endif  // SIMULATOR_H