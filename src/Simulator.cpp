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
        Console::clearScreen();
        cout << "Proceso No. " << i + 1 << endl << endl;
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

void Simulator::printRunningState() {
    Console::clearScreen();

    int size = batches.size();
    int pendingBatches = size - currentBatchIndex;

    cout << "No. Lotes pendientes: " << pendingBatches << endl << endl;

    Batch* currentBatch = batches[currentBatchIndex];
    int currentJobCount = currentBatch->getJobCount();

    // ----- LOTE ACTUAL -----
    cout << "--- Lote actual " << currentBatchIndex + 1 << " ---" << endl;
    cout << left << setw(15) << "Nombre" << setw(5) << "TME" << endl;

    for (int i = currentJobIndex + 1; i < currentJobCount; i++) {
        Job* pendingJob = currentBatch->getJob(i);
        cout << left << setw(15) << pendingJob->getName() << setw(5)
             << pendingJob->getEstimatedTime() << endl;
    }

    // ----- PROCESO ACTUAL -----
    if (currentJobIndex < currentJobCount) {
        Job* currentJob = currentBatch->getJob(currentJobIndex);

        cout << endl << "--- Proceso Actual ---" << endl;
        cout << "Nombre: " << currentJob->getName() << endl;
        cout << "Ope: " << currentJob->getOperation() << endl;
        cout << "TME: " << currentJob->getEstimatedTime() << endl;
        cout << "TT: " << currentJob->getElapsedTime() << endl;
        cout << "TR: " << currentJob->getRemainingTime() << endl;
    }

    // ----- TERMINADOS -----
    cout << "\n--- Terminados ---\n";
    cout << left << setw(5) << "ID" << setw(10) << "Ope" << setw(10) << "Res"
         << setw(10) << "Lote" << endl;

    for (int i = 0; i <= currentBatchIndex; i++) {
        Batch* batch = batches[i];
        int jobCount = batch->getJobCount();
        int limit = (i == currentBatchIndex) ? currentJobIndex : jobCount;

        for (int j = 0; j < limit; j++) {
            Job* job = batch->getJob(j);
            cout << left << setw(5) << job->getID() << setw(10)
                 << job->getOperation() << setw(10) << job->getResult()
                 << setw(10) << i + 1 << endl;
        }
    }

    cout << endl << "Contador: " << globalCounter << endl;
}

void Simulator::printFinalState() {
    Console::clearScreen();
    int pendingBatches = currentBatchIndex - (int)batches.size();

    cout << "No. Lotes pendientes: " << pendingBatches << endl << endl;
    cout << "Todos los procesos han terminado." << endl << endl;

    cout << "--- Resumen Final ---" << endl;
    cout << left << setw(5) << "ID" << setw(10) << "Ope" << setw(10) << "Res"
         << setw(10) << "Lote" << endl;

    for (size_t i = 0; i < batches.size(); i++) {
        Batch* batch = batches[i];
        int jobCount = batch->getJobCount();

        for (int j = 0; j < jobCount; j++) {
            Job* job = batch->getJob(j);
            cout << left << setw(5) << job->getID() << setw(10)
                 << job->getOperation() << setw(10) << job->getResult()
                 << setw(10) << i + 1 << endl;
        }
    }

    cout << endl << "Contador final: " << globalCounter << endl << endl;
}

void Simulator::printBatches() {
    int size = batches.size();
    if (currentBatchIndex < size) {
        printRunningState();
    } else {
        printFinalState();
    }
}