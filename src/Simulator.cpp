#include "Simulator.hpp"

#include <iomanip>
#include <iostream>

#include "ConsoleUtils.hpp"

using namespace std;

Simulator::Simulator() { this->globalCounter = 0; }

bool Simulator::isValidID(int id) {
    if (id <= 0) return false;

    size_t size = registeredIDs.size();
    for (int i = 0; i < (int)size; i++) {
        if (id == registeredIDs[i]) return false;
    }

    return true;
}

void Simulator::generateJobs(int numJobs) {
    for (int i = 0; i < numJobs; i++) {
        Job* job = jobManager.generateJob(this);
        job->setState(NEW);
        registeredIDs.push_back(job->getID());
        memory.insert(JOB_QUEUE, job);
    }
}

void Simulator::loadJobsToMemory() {
    while (!memory.isActiveQueueFull() && !memory.isEmpty(JOB_QUEUE)) {
        Job* job = memory.getJob(JOB_QUEUE, 0);
        if (job != nullptr) job->setState(READY);
        job->setArrivalTime(this->globalCounter);
        memory.moveJob(JOB_QUEUE, ACTIVE_QUEUE, 0);
    }
}

void Simulator::updateBlockedJobs() {
    int activeCount = memory.getJobCount(ACTIVE_QUEUE);
    for (int i = 0; i < activeCount; i++) {
        Job* job = memory.getJob(ACTIVE_QUEUE, i);
        if (job->getState() == BLOCKED) {
            job->passBlockedTime();
            if (job->getBlockedTime() == 0) job->setState(READY);
        }
    }
}

void Simulator::calculateFinalTimes(Job* job) {
    job->setCompletionTime(this->globalCounter);
    job->setServiceTime(job->getElapsedTime());
    job->setReturnTime(job->getCompletionTime() - job->getArrivalTime());
    job->setWaitingTime(job->getReturnTime() - job->getServiceTime());
}

void Simulator::run() {
    Console::TerminalMode termMode;
    while (!memory.isEmpty(JOB_QUEUE) || !memory.isEmpty(ACTIVE_QUEUE)) {
        loadJobsToMemory();

        if (!memory.isEmpty(ACTIVE_QUEUE)) {
            bool foundReady = false;
            int activeCount = memory.getJobCount(ACTIVE_QUEUE);

            for (int i = 0; i < activeCount; i++) {
                if (memory.getJob(ACTIVE_QUEUE, 0)->getState() == READY) {
                    foundReady = true;
                    break;
                } else {
                    memory.rotateActiveQueue();
                }
            }

            if (foundReady) {
                Job* currentJob = memory.getJob(ACTIVE_QUEUE, 0);

                if (currentJob->getResponseTime() == -1) {
                    currentJob->setResponseTime(this->globalCounter - currentJob->getArrivalTime());
                }

                currentJob->setState(RUNNING);
                int quantumCnt = 0;

                while (currentJob->getState() == RUNNING) {
                    render();
                    Console::sleep(1);
                    currentJob->passTime();
                    globalCounter++;
                    quantumCnt++;

                    updateBlockedJobs();

                    if (Console::keyPressed()) {
                        char key = toupper(Console::getKey());
                        switch (key) {
                            case 'I':
                                currentJob->setState(BLOCKED);
                                currentJob->setBlockedTime(BLOCKED_TIME);
                                memory.rotateActiveQueue();
                                break;
                            case 'E':
                                currentJob->setState(ERROR);
                                calculateFinalTimes(currentJob);
                                memory.moveJob(ACTIVE_QUEUE, TERMINATED_LOG, 0);
                                break;
                            case 'N':
                                generateJobs(1);
                                loadJobsToMemory();
                                break;
                            case 'B':
                                printBCPTable();
                                break;
                            case 'P':
                                bool paused = true;
                                while (paused) {
                                    while (Console::keyPressed()) {
                                        if (toupper(Console::getKey()) == 'C') paused = false;
                                    }
                                    if (paused) Console::sleep(1);
                                }
                                break;
                        }
                    }

                    if (currentJob->getState() != RUNNING) break;

                    if (currentJob->getElapsedTime() == currentJob->getEstimatedTime()) {
                        currentJob->calculateResult();
                        currentJob->setState(TERMINATED);
                        calculateFinalTimes(currentJob);
                        memory.moveJob(ACTIVE_QUEUE, TERMINATED_LOG, 0);
                        break;
                    }

                    if (quantumCnt == this->quantum) {
                        currentJob->setState(READY);
                        memory.rotateActiveQueue();
                        break;
                    }
                }
            } else {
                render();
                Console::sleep(1);
                updateBlockedJobs();
                globalCounter++;

                if (Console::keyPressed()) {
                    char key = toupper(Console::getKey());
                    switch (key) {
                        case 'N':
                            generateJobs(1);
                            loadJobsToMemory();
                            break;
                        case 'B':
                            printBCPTable();
                            break;
                        case 'P':
                            bool paused = true;
                            while (paused) {
                                while (Console::keyPressed()) {
                                    if (toupper(Console::getKey()) == 'C') paused = false;
                                }
                                if (paused) Console::sleep(1);
                            }
                            break;
                    }
                }
            }
        }
    }

    render();
    Console::pause();
}

void Simulator::askData() {
    int n;
    cout << "Ingrese el numero de procesos inicial: ";
    cin >> n;
    cout << "Ingrese el valor del quantum: ";
    cin >> this->quantum;
    cin.get();
    generateJobs(n);
}

void Simulator::centerText(string text, int width) {
    int totalLength = text.length() + 2;
    int x = (width - totalLength) / 2;
    for (int i = 0; i < x; i++) {
        cout << "-";
    }
    cout << " " << text << " ";
    for (int i = 0; i < (width - x - totalLength); i++) {
        cout << "-";
    }
    cout << endl;
}

void Simulator::printRunningState() {
    Console::clearScreen();
    int width = W_ID + W_OPE + W_RES;

    int pendingJobs = memory.getJobCount(JOB_QUEUE);
    cout << "No. Procesos en espera: " << pendingJobs << endl << endl;

    int activeJobCount = memory.getJobCount(ACTIVE_QUEUE);

    // ----- PROCESOS LISTOS (READY) -----
    centerText("Procesos en memoria", width);
    cout << left << setw(W_ID) << "ID" << setw(W_TIME) << "TME" << setw(W_TIME) << "TT" << endl;

    for (int i = 0; i < activeJobCount; i++) {
        Job* pendingJob = memory.getJob(ACTIVE_QUEUE, i);
        if (pendingJob->getState() == READY) {
            cout << left << setw(W_ID) << pendingJob->getID() << setw(W_TIME)
                 << pendingJob->getEstimatedTime() << setw(W_TIME) << pendingJob->getElapsedTime()
                 << endl;
        }
    }

    // ----- PROCESO ACTUAL (RUNNING) -----
    cout << endl;
    centerText("Proceso Actual", width);
    bool isCpuIdle = true;

    for (int i = 0; i < activeJobCount; i++) {
        Job* job = memory.getJob(ACTIVE_QUEUE, i);
        if (job->getState() == RUNNING) {
            cout << "ID: " << job->getID() << endl;
            cout << "OPE: " << job->getOperation() << endl;
            cout << "TME: " << job->getEstimatedTime() << endl;
            cout << "TT: " << job->getElapsedTime() << endl;

            int total = job->getEstimatedTime();
            int elapsed = job->getElapsedTime();
            int percentage = (total > 0) ? (elapsed * 100) / total : 0;
            int barWidth = 20;
            int filledWidth = (total > 0) ? (barWidth * elapsed) / total : 0;
            cout << "[";
            for (int p = 0; p < barWidth; ++p) {
                if (p < filledWidth) {
                    cout << "=";
                } else if (p == filledWidth && elapsed != total) {
                    cout << ">";
                } else {
                    cout << " ";
                }
            }
            cout << "] " << percentage << "%" << endl;

            isCpuIdle = false;
            break;
        }
    }

    if (isCpuIdle) {
        cout << "[Esperando procesos...]" << endl;
    }

    // ----- BLOQUEADOS (BLOCKED) -----
    bool hasBlockedJobs = false;
    for (int i = 0; i < activeJobCount; i++) {
        if (memory.getJob(ACTIVE_QUEUE, i)->getState() == BLOCKED) {
            hasBlockedJobs = true;
            break;
        }
    }
    if (hasBlockedJobs) {
        cout << endl;
        centerText("Procesos Bloqueados", width);
        cout << left << setw(W_ID) << "ID" << setw(W_TIME) << "TTB" << endl;

        for (int i = 0; i < activeJobCount; i++) {
            Job* blockedJob = memory.getJob(ACTIVE_QUEUE, i);
            if (blockedJob->getState() == BLOCKED) {
                cout << left << setw(W_ID) << blockedJob->getID() << setw(W_TIME)
                     << 8 - blockedJob->getBlockedTime() << endl;
            }
        }
    }

    // ----- TERMINADOS -----
    cout << endl;
    centerText("Terminados", width);
    cout << left << setw(W_ID) << "ID" << setw(W_OPE) << "Ope" << setw(W_RES) << "Res" << endl;

    int terminatedCount = memory.getJobCount(TERMINATED_LOG);
    for (int i = 0; i < terminatedCount; i++) {
        Job* job = memory.getJob(TERMINATED_LOG, i);
        State st = job->getState();

        cout << left << setw(W_ID) << job->getID() << setw(W_OPE) << job->getOperation()
             << setw(W_RES);

        if (st == TERMINATED) {
            cout << job->getResult();
        } else if (st == ERROR) {
            cout << "ERROR";
        }
        cout << endl;
    }

    cout << endl << "Contador: " << globalCounter << endl << endl;
    cout
        << "\"I\" - Interrumpir, \"E\" - Error, \"P\" - Pausar, \"C\" - Continuar, \"N\" - Generar "
           "nuevo proceso, \"B\" - Mostrar tabla BCP"
        << endl;
}

void Simulator::printFinalState() {
    Console::clearScreen();
    int width = W_ID + W_OPE + W_RES + (W_TIME * 6);

    cout << "Todos los procesos han terminado." << endl << endl;

    centerText("Resumen Final", width);

    cout << left << setw(W_ID) << "ID" << setw(W_OPE) << "Operacion" << setw(W_RES) << "Resultado"
         << setw(W_TIME) << "Llegada" << setw(W_TIME) << "Fin" << setw(W_TIME) << "Retorno"
         << setw(W_TIME) << "Resp" << setw(W_TIME) << "Espera" << setw(W_TIME) << "Servicio"
         << endl;

    for (int i = 0; i < (W_ID + W_OPE + W_RES + (W_TIME * 6)); i++) cout << "-";
    cout << endl;

    int terminatedCount = memory.getJobCount(TERMINATED_LOG);
    for (int i = 0; i < terminatedCount; i++) {
        Job* job = memory.getJob(TERMINATED_LOG, i);

        cout << left << setw(W_ID) << job->getID() << setw(W_OPE) << job->getOperation();

        if (job->getState() == TERMINATED) {
            cout << setw(W_RES) << job->getResult();
        } else if (job->getState() == ERROR) {
            cout << setw(W_RES) << "ERROR";
        }

        cout << setw(W_TIME) << job->getArrivalTime() << setw(W_TIME) << job->getCompletionTime()
             << setw(W_TIME) << job->getReturnTime() << setw(W_TIME) << job->getResponseTime()
             << setw(W_TIME) << job->getWaitingTime() << setw(W_TIME) << job->getServiceTime()
             << endl;
    }

    cout << endl << "Contador final: " << globalCounter << endl << endl;
}

void Simulator::render() {
    if (!memory.isEmpty(JOB_QUEUE) || !memory.isEmpty(ACTIVE_QUEUE)) {
        printRunningState();
    } else {
        printFinalState();
    }
}

void Simulator::printBCPTable() {
    Console::clearScreen();
    int bcpWidth = W_ID + 14 + W_OPE + W_RES + (W_TIME * 7);
    centerText("Tabla de Procesos (BCP)", bcpWidth);

    cout << left << setw(W_ID) << "ID" << setw(14) << "Estado" << setw(W_OPE) << "Ope"
         << setw(W_RES) << "Res" << setw(W_TIME) << "Llegada" << setw(W_TIME) << "Fin"
         << setw(W_TIME) << "Retorno" << setw(W_TIME) << "Espera" << setw(W_TIME) << "Serv"
         << setw(W_TIME) << "Rest" << setw(W_TIME) << "Resp" << endl;

    for (int i = 0; i < bcpWidth; i++) cout << "-";
    cout << endl;

    auto printJob = [&](Job* job) {
        State st = job->getState();

        cout << left << setw(W_ID) << job->getID();

        string estadoStr = "";
        switch (st) {
            case NEW:
                estadoStr = "NUEVO";
                break;
            case READY:
                estadoStr = "LISTO";
                break;
            case RUNNING:
                estadoStr = "EJECUCION";
                break;
            case BLOCKED:
                estadoStr = "BLOQ (" + to_string(job->getBlockedTime()) + ")";
                break;
            case TERMINATED:
                estadoStr = "TERMINADO";
                break;
            case ERROR:
                estadoStr = "ERROR";
                break;
        }
        cout << setw(14) << estadoStr;

        cout << setw(W_OPE) << job->getOperation();

        if (st == TERMINATED)
            cout << setw(W_RES) << job->getResult();
        else if (st == ERROR)
            cout << setw(W_RES) << "ERROR";
        else
            cout << setw(W_RES) << "N/A";

        if (st == NEW) {
            cout << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A"
                 << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A"
                 << setw(W_TIME) << "N/A" << endl;
        } else {
            cout << setw(W_TIME) << job->getArrivalTime();

            if (st == TERMINATED || st == ERROR) {
                cout << setw(W_TIME) << job->getCompletionTime() << setw(W_TIME)
                     << job->getReturnTime();
            } else {
                cout << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A";
            }

            int espera = (st == TERMINATED || st == ERROR)
                             ? job->getWaitingTime()
                             : (globalCounter - job->getArrivalTime() - job->getElapsedTime());
            cout << setw(W_TIME) << espera;

            cout << setw(W_TIME) << job->getElapsedTime();

            if (st == TERMINATED || st == ERROR) {
                cout << setw(W_TIME) << "N/A";
            } else {
                cout << setw(W_TIME) << (job->getEstimatedTime() - job->getElapsedTime());
            }

            if (job->getResponseTime() != -1) {
                cout << setw(W_TIME) << job->getResponseTime() << endl;
            } else {
                cout << setw(W_TIME) << "N/A" << endl;
            }
        }
    };

    for (int i = 0; i < memory.getJobCount(JOB_QUEUE); i++) printJob(memory.getJob(JOB_QUEUE, i));
    for (int i = 0; i < memory.getJobCount(ACTIVE_QUEUE); i++)
        printJob(memory.getJob(ACTIVE_QUEUE, i));
    for (int i = 0; i < memory.getJobCount(TERMINATED_LOG); i++)
        printJob(memory.getJob(TERMINATED_LOG, i));

    cout << endl << "Contador actual: " << globalCounter << endl;
    cout << "\nPresione 'C' para continuar..." << endl;

    bool paused = true;
    while (paused) {
        while (Console::keyPressed()) {
            if (toupper(Console::getKey()) == 'C') paused = false;
        }
        if (paused) Console::sleep(1);
    }
}
