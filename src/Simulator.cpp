#include "Simulator.hpp"

#include <iomanip>
#include <iostream>

#include "ConsoleUtils.hpp"

using namespace std;

Simulator::Simulator() { this->globalCounter = 0; }

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

void Simulator::createBatches(int numJobs) {
    JobManager jobManager;
    Batch* batch = new Batch();
    for (int i = 0; i < numJobs; i++) {
        Console::clearScreen();
        cout << "Proceso No. " << i + 1 << endl << endl;
        Job* job = jobManager.captureJob(this);
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
        int estimatedTime = job->getEstimatedTime();
        for (int j = 0; j < estimatedTime; j++) {
            Console::sleep(1);
            job->passTime();
            globalCounter++;
            render();
        }
        job->calculateResult();
        render();
        currentJobIndex++;
    }
}

void Simulator::processBatches() {
    this->currentBatchIndex = 0;
    int totalBatches = batches.size();
    while (currentBatchIndex < totalBatches) {
        processBatch(currentBatchIndex);
        currentBatchIndex++;
    }
    render();
    Console::pause();
}

void Simulator::centerText(string text) {
    int totalLength = text.length() + 2;
    int x = (WIDTH - totalLength) / 2;
    for (int i = 0; i < x; i++) {
        cout << "-";
    }
    cout << " " << text << " ";
    for (int i = 0; i < (WIDTH - x - totalLength); i++) {
        cout << "-";
    }
    cout << endl;
}

void Simulator::printRunningState() {
    Console::clearScreen();

    int pendingBatches = ((int)batches.size() - currentBatchIndex) - 1;

    cout << "No. Lotes pendientes: " << pendingBatches << endl << endl;

    Batch* currentBatch = batches[currentBatchIndex];
    int currentJobCount = currentBatch->getJobCount();

    // ----- LOTE ACTUAL -----
    centerText("Lote actual");
    cout << left << setw(COL_NAME) << "Nombre" << setw(COL_TME) << "TME"
         << endl;

    for (int i = currentJobIndex + 1; i < currentJobCount; i++) {
        Job* pendingJob = currentBatch->getJob(i);
        cout << left << setw(COL_NAME) << pendingJob->getName() << setw(COL_TME)
             << pendingJob->getEstimatedTime() << endl;
    }

    // ----- PROCESO ACTUAL -----
    if (currentJobIndex < currentJobCount) {
        Job* currentJob = currentBatch->getJob(currentJobIndex);

        cout << endl;
        centerText("Proceso Actual");
        cout << "Nombre: " << currentJob->getName() << endl;
        cout << "Ope: " << currentJob->getOperation() << endl;
        cout << "TME: " << currentJob->getEstimatedTime() << endl;
        cout << "TT: " << currentJob->getElapsedTime() << endl;
        cout << "TR: " << currentJob->getRemainingTime() << endl;
    }

    // ----- TERMINADOS -----
    cout << endl;
    centerText("Terminados");
    cout << left << setw(COL_ID) << "ID" << setw(COL_OPE) << "Ope"
         << setw(COL_RES) << "Res" << setw(COL_BAT) << "No. Lote" << endl;

    for (int i = 0; i <= currentBatchIndex; i++) {
        Batch* batch = batches[i];
        int jobCount = batch->getJobCount();
        int limit = (i == currentBatchIndex) ? currentJobIndex : jobCount;

        for (int j = 0; j < limit; j++) {
            Job* job = batch->getJob(j);
            cout << left << setw(COL_ID) << job->getID() << setw(COL_OPE)
                 << job->getOperation() << setw(COL_RES) << job->getResult()
                 << setw(COL_BAT) << i + 1 << endl;
        }

        if (i < currentBatchIndex) {
            for (int j = 0; j < WIDTH; j++) {
                cout << "-";
            }
            cout << endl;
        }
    }

    cout << endl << "Contador: " << globalCounter << endl;
}

void Simulator::printFinalState() {
    Console::clearScreen();
    int pendingBatches = currentBatchIndex - (int)batches.size();

    cout << "No. Lotes pendientes: " << pendingBatches << endl << endl;
    cout << "Todos los procesos han terminado." << endl << endl;

    centerText("Resumen Final");
    cout << left << setw(COL_ID) << "ID" << setw(COL_OPE) << "Ope"
         << setw(COL_RES) << "Res" << setw(COL_BAT) << "No. Lote" << endl;

    for (int i = 0; i < (int)batches.size(); i++) {
        Batch* batch = batches[i];
        int jobCount = batch->getJobCount();

        for (int j = 0; j < jobCount; j++) {
            Job* job = batch->getJob(j);
            cout << left << setw(COL_ID) << job->getID() << setw(COL_OPE)
                 << job->getOperation() << setw(COL_RES) << job->getResult()
                 << setw(COL_BAT) << i + 1 << endl;
        }

        if (i < currentBatchIndex - 1) {
            for (int j = 0; j < WIDTH; j++) {
                cout << "-";
            }
            cout << endl;
        }
    }

    cout << endl << "Contador final: " << globalCounter << endl << endl;
}

void Simulator::render() {
    if (currentBatchIndex < (int)batches.size()) {
        printRunningState();
    } else {
        printFinalState();
    }
}