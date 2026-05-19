#ifndef MEMORY_H
#define MEMORY_H

#include <vector>

#include "Job.hpp"

enum JobLocation { JOB_QUEUE, ACTIVE_QUEUE, TERMINATED_LOG };

struct Frame {
    int frameID;
    int jobID = -1;
    int pageID = -1;
    int usedSpace = 0;
    State state = NEW;
};

class Memory {
   private:
    std::vector<Frame> frames;
    std::vector<Job*> jobQueue;
    std::vector<Job*> activeQueue;
    std::vector<Job*> terminatedLog;

   public:
    Memory();
    int getFreeFrames();
    void updateFramesState(int jobID, State newState);
    void allocateJob(Job* job);
    void freeFrameJobs(int jobID);
    ~Memory();
    Frame& getFrame(int index);
    Job* getJob(JobLocation location, int position);
    int getJobCount(JobLocation location);
    bool isActiveQueueFull();
    bool isEmpty(JobLocation location);
    bool insert(JobLocation location, Job* job);
    bool moveJob(JobLocation origin, JobLocation destination, int position);
    void rotateActiveQueue();
};

#endif  // MEMORY_H