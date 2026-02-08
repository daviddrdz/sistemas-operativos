#include "Simulator.h"

#include <iomanip>
#include <iostream>

#include "ConsoleUtils.h"

using namespace std;

Simulator::~Simulator() {
    for (Batch* batch : batches) {
        int jobCount = batch->getJobCount();
        for (int j = 0; j < jobCount; j++) {
            delete batch->getJob(j);
        }
        delete batch;
    }
}

bool Simulator::isValidID(int id) {
    if (id <= 0) {
        return false;
    }

    size_t size = registeredIDs.size();
    for (size_t i = 0; i < size; i++) {
        if (id == registeredIDs[i]) {
            return false;
        }
    }

    return true;
}

void Simulator::createBatches(int numJobs, JobManager* jobManager) {
    Batch* batch = new Batch();
    for (int i = 0; i < numJobs; i++) {
        Job* job = jobManager->captureJob(this);
        batch->insert(job);
        registeredIDs.push_back(job->getID());
        if (batch->isFull()) {
            batches.push_back(batch);
            batch = new Batch;
        }
    }

    if (!batch->isEmpty()) {
        batches.push_back(batch);
    } else {
        delete batch;
    }
}

void Simulator::processBatch(int batch) {
    this->currentJobIndex = 0;
    int jobCounter = batches[batch]->getJobCount();
    while (currentJobIndex < jobCounter) {
        Job* job = batches[batch]->getJob(currentJobIndex);
        int tme = job->getEstimatedTime();
        for (int j = 0; j < tme; j++) {
            Console::sleep(1);
            job->passTime();
            globalCounter++;
            printBatches();
        }
        job->calculateResult();
        printBatches();
        currentJobIndex++;
    }
}

void Simulator::processBatches() {
    this->globalCounter = 0;
    this->currentBatchIndex = 0;
    int totalBatches = batches.size();
    while (currentBatchIndex < totalBatches) {
        processBatch(currentBatchIndex);
        currentBatchIndex++;
    }
    printBatches();
    Console::pause();
}

void Simulator::printBatches() {
    Console::clearScreen();
    int size = batches.size();
    int pendingBatches = size - currentBatchIndex;
    cout << "No. Lotes pendientes: " << pendingBatches << endl << endl;

    if (currentBatchIndex < size) {
        Batch* currentBatch = batches[currentBatchIndex];
        int currentJobCount = currentBatch->getJobCount();
        if (currentJobIndex < currentJobCount) {
            Job* currentJob =
                batches[currentBatchIndex]->getJob(currentJobIndex);

            cout << "Lote actual " << currentBatchIndex + 1 << endl;
            cout << left << setw(15) << "Nombre" << setw(5) << "TME" << endl;
            for (int i = currentJobIndex + 1; i < currentJobCount; i++) {
                Job* pendingJob = currentBatch->getJob(i);
                cout << setw(15) << pendingJob->getName() << setw(5)
                     << pendingJob->getEstimatedTime() << endl;
            }

            cout << endl << "Proceso Actual" << endl;
            cout << "Nombre: " << currentJob->getName() << endl;
            cout << "Ope: " << currentJob->getOperation() << endl;
            cout << "TME: " << currentJob->getEstimatedTime() << endl;
            cout << "TT: " << currentJob->getElapsedTime() << endl;
            cout << "TR: " << currentJob->getRemainingTime() << endl << endl;

            cout << "Terminados" << endl;
            cout << left << setw(5) << "ID" << setw(10) << "Ope" << setw(10)
                 << "Res" << setw(10) << "No. Lote" << endl;
            for (int i = 0; i <= currentBatchIndex; i++) {
                Batch* batch = batches[i];
                int jobCount = batch->getJobCount();
                int limit =
                    (i == currentBatchIndex) ? currentJobIndex : jobCount;
                for (int j = 0; j < limit; j++) {
                    Job* terminatedJob = batch->getJob(j);
                    cout << setw(5) << terminatedJob->getID() << setw(10)
                         << terminatedJob->getOperation() << setw(10)
                         << terminatedJob->getResult() << setw(10) << i + 1
                         << endl;
                }
            }

            cout << endl << "Contador: " << globalCounter;
        }
    } else {
        cout << "Todos los procesos han terminado" << endl << endl;
    }
}