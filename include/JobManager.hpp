#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <random>

#include "Job.hpp"

class Simulator;

enum OperationType { ADD, SUB, MULT, DIV, POW, MOD };

class JobManager {
   private:
    std::mt19937 generator;
    bool isValidOperation(std::string operation);

   public:
    JobManager();
    Job* generateJob(Simulator* simulator);
};

#endif  // JOB_MANAGER_H