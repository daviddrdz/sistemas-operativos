#include "Memory.hpp"

Memory::~Memory() {
    for (Job* job : jobQueue) delete job;
    for (Job* job : activeQueue) delete job;
    for (Job* job : terminatedLog) delete job;
}

Job* Memory::getJob(JobLocation location, int position) {
    switch (location) {
        case JOB_QUEUE:
            if (0 <= position && position < (int)jobQueue.size()) return jobQueue[position];
            break;
        case ACTIVE_QUEUE:
            if (0 <= position && position < (int)activeQueue.size()) return activeQueue[position];
            break;
        case TERMINATED_LOG:
            if (0 <= position && position < (int)terminatedLog.size())
                return terminatedLog[position];
            break;
    }
    return nullptr;
}

int Memory::getJobCount(JobLocation location) {
    switch (location) {
        case JOB_QUEUE:
            return static_cast<int>(jobQueue.size());
        case ACTIVE_QUEUE:
            return static_cast<int>(activeQueue.size());
        case TERMINATED_LOG:
            return static_cast<int>(terminatedLog.size());
        default:
            return 0;
    }
}

bool Memory::isActiveQueueFull() { return activeQueue.size() >= MAX_ACTIVE_JOBS; }

bool Memory::isEmpty(JobLocation location) {
    switch (location) {
        case JOB_QUEUE:
            return jobQueue.empty();
        case ACTIVE_QUEUE:
            return activeQueue.empty();
        case TERMINATED_LOG:
            return terminatedLog.empty();
        default:
            return true;
    }
}

bool Memory::insert(JobLocation location, Job* job) {
    if (job == nullptr) return false;

    switch (location) {
        case JOB_QUEUE:
            jobQueue.push_back(job);
            return true;
        case ACTIVE_QUEUE:
            if (!isActiveQueueFull()) {
                activeQueue.push_back(job);
                return true;
            }
            return false;
        case TERMINATED_LOG:
            terminatedLog.push_back(job);
            return true;
        default:
            return false;
    }
}

bool Memory::moveJob(JobLocation origin, JobLocation destination, int position) {
    Job* job = getJob(origin, position);
    if (job == nullptr) return false;
    bool inserted = insert(destination, job);
    if (inserted) {
        switch (origin) {
            case JOB_QUEUE:
                jobQueue.erase(jobQueue.begin() + position);
                break;
            case ACTIVE_QUEUE:
                activeQueue.erase(activeQueue.begin() + position);
                break;
            case TERMINATED_LOG:
                terminatedLog.erase(terminatedLog.begin() + position);
                break;
        }
        return true;
    }
    return false;
}

void Memory::rotateActiveQueue() {
    if (!activeQueue.empty()) {
        Job* firstJob = activeQueue.front();
        activeQueue.erase(activeQueue.begin());
        activeQueue.push_back(firstJob);
    }
}