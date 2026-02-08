#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include "Job.h"
#include "Simulator.h"

enum OperationType { ADD, SUB, MULT, DIV, POW };

class Simulator;

class JobManager {
   private:
    bool isValidOperation(std::string operation);

   public:
    Job* captureJob(Simulator* simulator);
};

#endif  // JOB_MANAGER_H