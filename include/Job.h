#ifndef JOB_H
#define JOB_H

#include <string>

using namespace std;

class Job {
   private:
    string name;
    string operation;
    int estimatedTime;
    int id;

   public:
    string getName() { return this->name; }
    int getID() { return this->id; }

    Job(string name, string operation, int estimatedTime, int id) {
        this->name = name;
        this->operation = operation;
        this->estimatedTime = estimatedTime;
        this->id = id;
    }
};

#endif  // JOB_H