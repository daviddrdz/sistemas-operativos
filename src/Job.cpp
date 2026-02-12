#include "Job.hpp"

#include <string>

using namespace std;

Job::Job(string name, string operation, int estimatedTime, int id) {
    this->name = name;
    this->operation = operation;
    this->estimatedTime = estimatedTime;
    this->elapsedTime = 0;
    this->remainingTime = this->estimatedTime;
    this->id = id;
    this->result = 0;
}

string Job::getName() { return this->name; }
string Job::getOperation() { return this->operation; }
int Job::getEstimatedTime() { return this->estimatedTime; }
int Job::getElapsedTime() { return this->elapsedTime; }
int Job::getRemainingTime() { return this->remainingTime; }
int Job::getID() { return this->id; }
float Job::getResult() { return this->result; }

void Job::passTime() {
    this->elapsedTime++;
    this->remainingTime--;
}

void Job::calculateResult() {
    int operatorIndex = -1;

    for (size_t i = 0; i < operation.size(); i++) {
        char c = operation[i];

        if (i == 0 && (c == '+' || c == '-')) continue;

        if (OPERATORS.find(c) != string::npos) {
            operatorIndex = i;
            break;
        }
    }

    char operatorSign = operation[operatorIndex];

    int x = stoi(operation.substr(0, operatorIndex));
    int y = stoi(operation.substr(operatorIndex + 1));

    switch (operatorSign) {
        case '+':
            result = x + y;
            break;
        case '-':
            result = x - y;
            break;
        case '*':
            result = x * y;
            break;
        case '/':
            result = x / y;
            break;
        case '%':
            result = x % y;
            break;
        case '^':
            if (y >= 0) {
                result = 1;
                for (int i = 0; i < y; i++) result *= x;
            } else {
                result = 1;
                for (int i = 0; i < -y; i++) result *= x;
                result = 1.0 / result;
            }
            break;
    }
}