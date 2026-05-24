#include "Memory.hpp"

Memory::Memory() : frames(46) {
    for(int i = 0; i < 46; ++i) {
        frames[i].frameID = i;
        frames[i].jobID = -1;      
        frames[i].pageID = -1;
        frames[i].usedSpace = 0;
    }
    
    for(int i = 41; i <= 45; ++i) {
        frames[i].jobID = 0;       
        frames[i].usedSpace = 5;
    }
}

Memory::~Memory() {
    for (Job* job : jobQueue) delete job;
    for (Job* job : activeQueue) delete job;
    for (Job* job : terminatedLog) delete job;
}

int Memory::getFreeFrames() {
    int count = 0;
    for (int i = 0; i < 41; ++i) {
        if (frames[i].jobID == -1) {
            count++;
        }
    }
    return count;
}

void Memory::updateFramesState(int jobID, State newState) {
    for(int i = 0; i < 41; ++i) {
        if(frames[i].jobID == jobID) {
            frames[i].state = newState;
        }
    }
}

void Memory::allocateJob(Job* job) {
    int pagesToAllocate = job->getPageCount();
    int remainingSize = job->getSize();
    
    for(int i=0; i<41 && pagesToAllocate > 0; ++i) {
        if(frames[i].jobID == -1) {
            frames[i].jobID = job->getID();
            frames[i].pageID = job->getPageCount() - pagesToAllocate;
            frames[i].usedSpace = (remainingSize >= 5) ? 5 : remainingSize;
            frames[i].state = READY;
            
            remainingSize -= 5;
            pagesToAllocate--;
        }
    }
}

void Memory::freeFrameJobs(int jobID) {
    for(int i=0; i<41; ++i) {
        if(frames[i].jobID == jobID) {
            frames[i].jobID = -1;
            frames[i].pageID = -1;
            frames[i].usedSpace = 0;
        }
    }
}

Frame& Memory::getFrame(int index) { return frames[index]; }

Job* Memory::getJob(JobLocation location, int position) {
    switch (location) {
        case JOB_QUEUE:
            if (0 <= position && position < (int)jobQueue.size()) return jobQueue[position];
            break;
        case ACTIVE_QUEUE:
            if (0 <= position && position < (int)activeQueue.size()) return activeQueue[position];
            break;
        case SUSPENDED_QUEUE:
            if (0 <= position && position < (int)suspendedQueue.size()) return suspendedQueue[position];
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
        case JOB_QUEUE: return static_cast<int>(jobQueue.size());
        case ACTIVE_QUEUE: return static_cast<int>(activeQueue.size());
        case SUSPENDED_QUEUE: return static_cast<int>(suspendedQueue.size());
        case TERMINATED_LOG: return static_cast<int>(terminatedLog.size());
        default: return 0;
    }
}

bool Memory::isActiveQueueFull() { return activeQueue.size() >= 5; }

bool Memory::isEmpty(JobLocation location) {
    switch (location) {
        case JOB_QUEUE: return jobQueue.empty();
        case ACTIVE_QUEUE: return activeQueue.empty();
        case SUSPENDED_QUEUE: return suspendedQueue.empty();
        case TERMINATED_LOG: return terminatedLog.empty();
        default: return true;
    }
}

bool Memory::insert(JobLocation location, Job* job) {
    if (job == nullptr) return false;

    switch (location) {
        case JOB_QUEUE:
            jobQueue.push_back(job);
            return true;
        case ACTIVE_QUEUE:
            activeQueue.push_back(job);
            return true;
        case SUSPENDED_QUEUE:
            suspendedQueue.push_back(job);
            return true;
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
            case SUSPENDED_QUEUE:
                suspendedQueue.erase(suspendedQueue.begin() + position);
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
