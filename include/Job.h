#ifndef JOB_H
#define JOB_H

#include <string>

const std::string OPERATORS = "+-*/^";

class Job {
   private:
    std::string name;
    std::string operation;
    int estimatedTime;
    int elapsedTime;
    int remainingTime;
    int id;
    float result;

   public:
    Job(std::string name, std::string operation, int tme, int id);
    std::string getName();
    std::string getOperation();
    int getEstimatedTime();
    int getElapsedTime();
    int getRemainingTime();
    int getID();
    void passTime();
    void calculateResult();
    float getResult();
};

#endif  // JOB_H