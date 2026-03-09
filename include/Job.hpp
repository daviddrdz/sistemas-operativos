#ifndef JOB_H
#define JOB_H

#include <string>

const std::string OPERATORS = "+-*/^%";
enum State { NEW, READY, RUNNING, BLOCKED, TERMINATED, ERROR };

class Job {
   private:
    int id;
    std::string operation;
    int estimatedTime;
    State state;
    float result;

    int arrivalTime;
    int completionTime;
    int returnTime;
    int responseTime;
    int waitingTime;
    int serviceTime;

    int elapsedTime;
    int remainingTime;
    int blockedTime;

   public:
    Job(int id, std::string operation, int estimatedTime);

    int getID();
    std::string getOperation();
    int getEstimatedTime();
    bool setState(State state);
    State getState();
    float getResult();

    bool setArrivalTime(int time);
    int getArrivalTime();
    bool setCompletionTime(int time);
    int getCompletionTime();
    bool setReturnTime(int time);
    int getReturnTime();
    bool setResponseTime(int time);
    int getResponseTime();
    bool setWaitingTime(int time);
    int getWaitingTime();
    bool setServiceTime(int time);
    int getServiceTime();

    int getElapsedTime();
    int getRemainingTime();
    bool setBlockedTime(int time);
    int getBlockedTime();

    void passTime();
    void passBlockedTime();
    void calculateResult();
};

#endif  // JOB_H