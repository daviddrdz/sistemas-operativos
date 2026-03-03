#include "Job.hpp"

#include <string>

using namespace std;

Job::Job(int id, string operation, int estimatedTime) {
    this->id = id;
    this->operation = operation;
    this->estimatedTime = estimatedTime;
    this->state = READY;
    this->elapsedTime = 0;
    this->remainingTime = this->estimatedTime;
    this->result = 0;
}

string Job::getOperation() { return this->operation; }
int Job::getEstimatedTime() { return this->estimatedTime; }
int Job::getElapsedTime() { return this->elapsedTime; }
int Job::getRemainingTime() { return this->remainingTime; }
int Job::getID() { return this->id; }
float Job::getResult() { return this->result; }
State Job::getState() { return this->state; }
bool Job::setState(State state) {
    if (state < READY || state > ERROR) {
        return false;
    } else {
        this->state = state;
        return true;
    }
}

void Job::passTime() {
    if (remainingTime > 0) {
        this->elapsedTime++;
        this->remainingTime--;
    }
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