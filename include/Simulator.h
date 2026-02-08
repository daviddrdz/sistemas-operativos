#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "Batch.h"
#include "JobManager.h"

const int COL_LOTE = 25;
const int COL_PROCESO = 30;
const int COL_TERM = 25;

class JobManager;

class Simulator {
   private:
    std::vector<Batch*> batches;
    std::vector<int> registeredIDs;
    int currentBatchIndex;
    int currentJobIndex;
    int globalCounter;

   public:
    ~Simulator();
    bool isValidID(int id);
    void createBatches(int numJobs, JobManager* jobManager);
    void processBatch(int batch);
    void processBatches();
    void printBatches();
};

#endif  // SIMULATOR_H