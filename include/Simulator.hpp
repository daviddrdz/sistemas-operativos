#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "Batch.hpp"
#include "JobManager.hpp"

const int COL_NAME = 15;
const int COL_TME = 5;
const int COL_ID = 5;
const int COL_OPE = 10;
const int COL_RES = 10;
const int COL_BAT = 10;
const int WIDTH = COL_ID + COL_OPE + COL_RES + COL_BAT;

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
    void createBatches(int numJobs);
    void processBatch(int batch);
    void processBatches();
    void centerText(std::string text);
    void printRunningState();
    void printFinalState();
    void render();
};

#endif  // SIMULATOR_H