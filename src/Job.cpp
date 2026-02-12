#include "Job.h"

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
    size_t operatorIndex = this->operation.find_first_of(OPERATORS);
    char operatorSign = this->operation.at(operatorIndex);
    int x = stoi(operation.substr(0, operatorIndex));
    int y = stoi(operation.substr(operatorIndex + 1));
    switch (operatorSign) {
        case '+':
            this->result = x + y;
            break;
        case '-':
            this->result = x - y;
            break;
        case '*':
            this->result = x * y;
            break;
        case '/':
            this->result = x / y;
            break;
        case '^':
            if (0 < y) {
                this->result = 1;
                for (int i = 0; i < y; i++) {
                    this->result *= x;
                }
            } else {
                this->result = 1;
            }
            break;
        case '%':
            this->result = x % y;
            break;
    }
}