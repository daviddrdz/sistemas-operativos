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
    if (id <= 0) return false;

    size_t size = registeredIDs.size();
    for (size_t i = 0; i < size; i++) {
        if (id == registeredIDs[i]) return false;
    }

    return true;
}

void Simulator::createBatches(int numJobs) {
    Batch* batch = new Batch();
    for (int i = 0; i < numJobs; i++) {
        Job* job = jobManager.generateJob(this);
        batch->insert(job);
        registeredIDs.push_back(job->getID());
        if (batch->isFull()) {
            batches.push_back(batch);
            batch = new Batch();
        }
    }

    if (!batch->isEmpty())
        batches.push_back(batch);
    else
        delete batch;
}

void Simulator::processBatch(int batchIndex) {
    Batch* batch = batches[batchIndex];
    while (batch->hasActiveJobs()) {
        Job* job = batch->getJob(0);
        if (job->getState() == FINISHED || job->getState() == ERROR) {
            batch->rotate();
            continue;
        }
        if (job->getState() == READY) {
            job->setState(RUNNING);
        }
        while (job->getState() == RUNNING) {
            render();
            Console::sleep(1);
            job->passTime();
            globalCounter++;

            if (Console::keyPressed()) {
                char key = tolower(Console::getKey());

                if (key == 'i') {
                    job->setState(READY);
                    batch->rotate();
                    break;
                } else if (key == 'e') {
                    job->setState(ERROR);
                    batch->rotate();
                    break;
                } else if (key == 'p') {
                    bool paused = true;
                    while (paused) {
                        if (Console::keyPressed() &&
                            tolower(Console::getKey()) == 'c') {
                            paused = false;
                        }
                        Console::sleep(1);
                    }
                }
            }

            if (job->getRemainingTime() <= 0) {
                job->calculateResult();
                job->setState(FINISHED);
                batch->rotate();
                break;
            }
        }
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
    cout << left << setw(COL_ID) << "ID" << setw(COL_TME) << "TME"
         << setw(COL_TME) << "TR" << endl;

    for (int i = 0; i < currentJobCount; i++) {
        Job* pendingJob = currentBatch->getJob(i);
        if (pendingJob->getState() == READY) {
            cout << left << setw(COL_ID) << pendingJob->getID() << setw(COL_TME)
                 << pendingJob->getEstimatedTime() << setw(COL_TME)
                 << pendingJob->getRemainingTime() << endl;
        }
    }

    // ----- PROCESO ACTUAL -----
    for (int i = 0; i < currentJobCount; i++) {
        Job* job = currentBatch->getJob(i);
        if (job->getState() == RUNNING) {
            cout << endl;
            centerText("Proceso Actual");
            cout << "ID: " << job->getID() << endl;
            cout << "Ope: " << job->getOperation() << endl;
            cout << "TME: " << job->getEstimatedTime() << endl;
            cout << "TT: " << job->getElapsedTime() << endl;
            cout << "TR: " << job->getRemainingTime() << endl;
            break;
        }
    }

    // ----- TERMINADOS -----
    cout << endl;
    centerText("Terminados");
    cout << left << setw(COL_ID) << "ID" << setw(COL_OPE) << "Ope"
         << setw(COL_RES) << "Res" << setw(COL_BAT) << "No. Lote" << endl;

    for (int i = 0; i <= currentBatchIndex; i++) {
        Batch* batch = batches[i];
        int jobCount = batch->getJobCount();
        for (int j = 0; j < jobCount; j++) {
            Job* job = batch->getJob(j);
            State st = job->getState();
            if (st == FINISHED || st == ERROR) {
                cout << left << setw(COL_ID) << job->getID() << setw(COL_OPE)
                     << job->getOperation() << setw(COL_RES);
                if (st == FINISHED) {
                    cout << job->getResult();
                } else if (st == ERROR) {
                    cout << "ERROR";
                }
                cout << setw(COL_BAT) << i + 1 << endl;
            }
        }
        if (i < currentBatchIndex) {
            for (int j = 0; j < WIDTH; j++) {
                cout << "-";
            }
            cout << endl;
        }
    }
    cout << endl << "Contador: " << globalCounter << endl << endl;
    cout << "\"I\" - Interrumpir, \"E\" - Error, \"P\" - Pausar, \"C\" - "
            "Continuar"
         << endl;
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
                 << job->getOperation() << setw(COL_RES);
            if (job->getState() == FINISHED) {
                cout << job->getResult();
            } else if (job->getState() == ERROR) {
                cout << "ERROR";
            }
            cout << setw(COL_BAT) << i + 1 << endl;
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