#include "JobManager.h"

#include <iostream>

#include "ConsoleUtils.h"
#include "Job.h"
#include "Simulator.h"
using namespace std;

bool JobManager::isValidOperation(string operation) {
    size_t operatorIndex = operation.find_first_of(OPERATORS);
    if (operatorIndex != string::npos) {
        int operand = stoi(operation.substr(operatorIndex + 1));
        if ((operation[operatorIndex] != OPERATORS[DIV]) ||
            (operation[operatorIndex] == OPERATORS[DIV] && operand != 0)) {
            return true;
        }
    }
    return false;
}

Job* JobManager::captureJob(Simulator* simulator) {
    string name, operation;
    Job* job;
    int estimatedTime, id;
    Console::clearScreen();
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
        cin.get();
    } while (!simulator->isValidID(id));
    job = new Job(name, operation, estimatedTime, id);
    Console::pause();

    return job;
}