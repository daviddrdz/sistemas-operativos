#ifndef JOB_H
#define JOB_H

#include <string>

const std::string OPERATORS = "+-*/^%";
enum State { READY, RUNNING, FINISHED, ERROR };

class Job {
   private:
    int id;
    std::string operation;
    int estimatedTime;
    State state;
    int elapsedTime;
    int remainingTime;
    float result;

   public:
    Job(int id, std::string operation, int estimatedTime);
    std::string getOperation();
    int getEstimatedTime();
    int getElapsedTime();
    int getRemainingTime();
    int getID();
    State getState();
    bool setState(State state);
    void passTime();
    void calculateResult();
    float getResult();
};

#endif  // JOB_H