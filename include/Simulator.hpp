#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "JobManager.hpp"
#include "Memory.hpp"

const std::string SUSPENDED_FILE = "suspended.txt";

const int BLOCKED_TIME = 6;
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
    int quantum;

   public:
    Simulator();
    bool isValidID(int id);
    void generateJobs(int numJobs);
    void loadJobsToMemory();
    void updateBlockedJobs();
    void suspendJob();
    void returnJob();

    void calculateFinalTimes(Job* job);
    std::string formatResult(Job* job);
    void handleRunningKeys(char key, Job* currentJob);
    void pauseSimulation();
    void executeRunningJob(Job* currentJob);
    void executeIdleCPU();
    void processActiveQueue();

    void run();

    void askData();
    void showPageTable();
    void centerText(std::string text, int width);
    void printRunningState();
    void printFinalState();
    void render();
    void printBCPTable();
    void printMemoryMap();
};

#endif  // SIMULATOR_H