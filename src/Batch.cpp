#include "Batch.h"

Batch::Batch() { this->jobCount = 0; }
int Batch::getJobCount() { return this->jobCount; }
Job* Batch::getJob(int position) {
    if (0 <= position && position < this->jobCount) {
        return batch[position];
    }
    return nullptr;
}

bool Batch::isFull() { return this->jobCount == MAX_JOBS_BATCH; }
bool Batch::isEmpty() { return this->jobCount == 0; }

bool Batch::insert(Job* job) {
    if (isFull()) {
        return false;
    }
    batch[jobCount++] = job;
    return true;
}