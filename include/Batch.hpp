#ifndef BATCH_H
#define BATCH_H

#include <vector>

#include "Job.hpp"

static const int MAX_JOBS_BATCH = 5;

class Batch {
   private:
    std::vector<Job*> batch;

   public:
    Job* getJob(int position);
    int getJobCount();
    bool isFull();
    bool isEmpty();
    bool insert(Job* job);
    bool hasActiveJobs();
    void rotate();
};

#endif  // BATCH_H