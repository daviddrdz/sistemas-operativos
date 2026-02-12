#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "Batch.h"
#include "JobManager.h"

const int WIDTH = 35;
const int COL_NAME = 15;
const int COL_TME = 5;
const int COL_ID = 5;
const int COL_OPE = 10;
const int COL_RES = 10;
const int COL_BAT = 10;

class JobManager;

class Simulator {
   private:
    std::vector<Batch*> batches;
    std::vector<int> registeredIDs;
    int currentBatchIndex;
    int currentJobIndex;
    int globalCounter;

   public:
    Simulator();
    ~Simulator();
    bool isValidID(int id);
    void createBatches(int numJobs, JobManager* jobManager);
    void processBatch(int batch);
    void processBatches();
    void centerText(std::string text);
    void printRunningState();
    void printFinalState();
    void render();
};

#endif  // SIMULATOR_H