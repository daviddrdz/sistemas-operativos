#ifndef BATCH_H
#define BATCH_H

#include "Job.hpp"

static const int MAX_JOBS_BATCH = 5;

class Batch {
   private:
    Job* batch[MAX_JOBS_BATCH];
    int jobCount;

   public:
    Batch();
    int getJobCount();
    Job* getJob(int position);
    bool isFull();
    bool isEmpty();
    bool insert(Job* job);
};

#endif  // BATCH_H