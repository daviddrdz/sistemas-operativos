#include "Batch.hpp"

Job* Batch::getJob(int position) {
    if (0 <= position && position < this->getJobCount()) {
        return batch[position];
    }
    return nullptr;
}

int Batch::getJobCount() { return batch.size(); }
bool Batch::isFull() { return batch.size() >= MAX_JOBS_BATCH; }
bool Batch::isEmpty() { return batch.empty(); }

bool Batch::insert(Job* job) {
    if (isFull()) return false;
    batch.push_back(job);
    return true;
}

bool Batch::hasActiveJobs() {
    for (int i = 0; i < (int)batch.size(); i++) {
        State currentJobState = batch[i]->getState();
        if (currentJobState == READY || currentJobState == RUNNING) return true;
    }
    return false;
}

void Batch::rotate() {
    if (batch.empty()) return;
    Job* current = batch[0];
    batch.erase(batch.begin());
    batch.push_back(current);
}