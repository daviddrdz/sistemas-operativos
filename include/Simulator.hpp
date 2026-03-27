#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "JobManager.hpp"
#include "Memory.hpp"

const int BLOCKED_TIME = 8;
const int W_ID = 6;
const int W_OPE = 12;
const int W_RES = 15;
const int W_TIME = 9;

const int WIDTH = W_ID + W_OPE + W_RES + (W_TIME * 6);

class Simulator {
   private:
    Memory memory;
    JobManager jobManager;
    std::vector<int> registeredIDs;
    int globalCounter;

   public:
    Simulator();
    bool isValidID(int id);
    void generateJobs(int numJobs);
    void loadJobsToMemory();
    void updateBlockedJobs();
    void calculateFinalTimes(Job* job);
    void run();

    void centerText(std::string text, int width);
    void printRunningState();
    void printFinalState();
    void render();
    void printBCPTable();
};

#endif  // SIMULATOR_H