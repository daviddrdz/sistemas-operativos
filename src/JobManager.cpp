#include "JobManager.hpp"

#include <iostream>

#include "ConsoleUtils.hpp"
#include "Job.hpp"
#include "Simulator.hpp"
using namespace std;

JobManager::JobManager() {
    std::random_device random;
    generator.seed(random());
}

bool JobManager::isValidOperation(string operation) {
    if (operation.empty()) return false;
    int operatorIndex = -1;
    int operatorCount = 0;
    for (size_t i = 0; i < operation.size(); i++) {
        char c = operation[i];
        if (i == 0 && (c == '+' || c == '-')) continue;
        if (OPERATORS.find(c) != string::npos) {
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

Job* JobManager::generateJob(Simulator* simulator) {
    uniform_int_distribution<> idDist(1, 999);
    uniform_int_distribution<> numDist(-100, 100);
    uniform_int_distribution<> baseDist(-10, 10);
    uniform_int_distribution<> exponentDist(0, 10);
    uniform_int_distribution<> opDist(0, (OPERATORS.size() - 1));
    uniform_int_distribution<> timeDist(5, 10);

    Job* job;
    int id, estimatedTime, num1, num2;
    char operatorChar;
    string operation;

    do {
        id = idDist(generator);
    } while (!simulator->isValidID(id));

    do {
        operatorChar = OPERATORS[opDist(generator)];
        if (operatorChar != '^') {
            num1 = numDist(generator);
            num2 = numDist(generator);
        } else {
            num1 = baseDist(generator);
            num2 = exponentDist(generator);
        }
        operation = to_string(num1) + operatorChar + to_string(num2);
    } while (!isValidOperation(operation));

    estimatedTime = timeDist(generator);

    job = new Job(id, operation, estimatedTime);

    return job;
}