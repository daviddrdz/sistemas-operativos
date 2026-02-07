#ifndef BATCH_H
#define BATCH_H

#include "Job.h"

#define MAX_JOBS_BATCH 5

class Batch {
   private:
    Job jobs[MAX_JOBS_BATCH];

   public:
    bool idExists(int id) {
        int jobID;
        for (int i = 0; i < MAX_JOBS_BATCH; i++) {
            if (jobs[i].getID() == id) {
                return true;
            }
        }
        return false;
    }
};

#endif  // BATCH_H