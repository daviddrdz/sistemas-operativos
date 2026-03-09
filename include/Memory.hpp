#ifndef MEMORY_H
#define MEMORY_H

#include <vector>

#include "Job.hpp"

enum JobLocation { JOB_QUEUE, ACTIVE_QUEUE, TERMINATED_LOG };

class Memory {
   private:
    static const int MAX_ACTIVE_JOBS = 5;
    std::vector<Job*> jobQueue;
    std::vector<Job*> activeQueue;
    std::vector<Job*> terminatedLog;

   public:
    ~Memory();
    Job* getJob(JobLocation location, int position);
    int getJobCount(JobLocation location);
    bool isActiveQueueFull();
    bool isEmpty(JobLocation location);
    bool insert(JobLocation location, Job* job);
    bool moveJob(JobLocation origin, JobLocation destination, int position);
    void rotateActiveQueue();
};

#endif  // MEMORY_H