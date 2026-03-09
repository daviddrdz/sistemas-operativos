#include "Job.hpp"

#include <string>

using namespace std;

Job::Job(int id, string operation, int estimatedTime) {
    this->id = id;
    this->operation = operation;
    this->estimatedTime = estimatedTime;
    this->state = NEW;
    this->result = 0;

    this->responseTime = -1;

    this->elapsedTime = 0;
    this->remainingTime = this->estimatedTime;
    this->blockedTime = 0;
}

int Job::getID() { return this->id; }
string Job::getOperation() { return this->operation; }
int Job::getEstimatedTime() { return this->estimatedTime; }
bool Job::setState(State state) {
    if (state < READY || state > ERROR) {
        return false;
    } else {
        this->state = state;
        return true;
    }
}
State Job::getState() { return this->state; }
float Job::getResult() { return this->result; }

bool Job::setArrivalTime(int time) {
    this->arrivalTime = time;
    return true;
}
int Job::getArrivalTime() { return this->arrivalTime; }
bool Job::setCompletionTime(int time) {
    this->completionTime = time;
    return true;
}
int Job::getCompletionTime() { return this->completionTime; }
bool Job::setReturnTime(int time) {
    this->returnTime = time;
    return true;
}
int Job::getReturnTime() { return this->returnTime; }
bool Job::setResponseTime(int time) {
    this->responseTime = time;
    return true;
}
int Job::getResponseTime() { return this->responseTime; }
bool Job::setWaitingTime(int time) {
    this->waitingTime = time;
    return true;
}
int Job::getWaitingTime() { return this->waitingTime; }
bool Job::setServiceTime(int time) {
    this->serviceTime = time;
    return true;
}
int Job::getServiceTime() { return this->serviceTime; }

int Job::getElapsedTime() { return this->elapsedTime; }
int Job::getRemainingTime() { return this->remainingTime; }
bool Job::setBlockedTime(int time) {
    this->blockedTime = time;
    return true;
}
int Job::getBlockedTime() { return this->blockedTime; }

void Job::passTime() {
    if (remainingTime > 0) {
        this->elapsedTime++;
        this->remainingTime--;
    }
}
void Job::passBlockedTime() {
    if (this->blockedTime > 0) {
        this->blockedTime--;
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