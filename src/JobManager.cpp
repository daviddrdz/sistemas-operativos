#include "JobManager.hpp"

#include <iostream>

#include "ConsoleUtils.hpp"
#include "Job.hpp"
#include "Simulator.hpp"
using namespace std;

bool JobManager::isValidOperation(string operation) {
    if (operation.empty()) return false;
    int operatorIndex = -1;
    int operatorCount = 0;
    for (size_t i = 0; i < operation.size(); i++) {
        char c = operation[i];
        if (i == 0 && (c == '+' || c == '-')) continue;
        if (string("+-*/%^").find(c) != string::npos) {
            operatorCount++;
            operatorIndex = i;
        }
    }

    if (operatorCount != 1) return false;
    if (operatorIndex == 0 || operatorIndex == (int)operation.size() - 1) {
        return false;
    }

    string left = operation.substr(0, operatorIndex);
    string right = operation.substr(operatorIndex + 1);

    int y = stoi(right);
    char op = operation[operatorIndex];
    if ((op == '/' || op == '%') && y == 0) return false;

    return true;
}

Job* JobManager::captureJob(Simulator* simulator) {
    string name, operation;
    Job* job;
    int estimatedTime, id;
    cout << "Nombre: ";
    getline(cin, name);
    do {
        cout << "Operation (" << OPERATORS << "): ";
        getline(cin, operation);
    } while (!isValidOperation(operation));
    do {
        cout << "Tiempo estimado (seg): ";
        cin >> estimatedTime;
    } while (estimatedTime <= 0);
    do {
        cout << "ID: ";
        cin >> id;
        cin.get();
    } while (!simulator->isValidID(id));
    job = new Job(name, operation, estimatedTime, id);
    cout << endl;
    Console::pause();

    return job;
}