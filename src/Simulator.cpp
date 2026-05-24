#include "Simulator.hpp"

#include <iomanip>
#include <iostream>
#include <sstream> 
#include <fstream>

#include "ConsoleUtils.hpp"

using namespace std;

Simulator::Simulator() { 
    this->globalCounter = 0;
    ofstream file(SUSPENDED_FILE, ios::trunc);
    if(file.is_open()) file.close();
}

bool Simulator::isValidID(int id) {
    if (id <= 0) return false;
    for (int registeredID : registeredIDs) {
        if (id == registeredID) return false;
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
    while (!memory.isEmpty(JOB_QUEUE)) {
        Job* nextJob = memory.getJob(JOB_QUEUE, 0);
        if (memory.getFreeFrames() >= nextJob->getPageCount()) {
            nextJob->setState(READY);
            nextJob->setArrivalTime(this->globalCounter);
            memory.allocateJob(nextJob);
            memory.moveJob(JOB_QUEUE, ACTIVE_QUEUE, 0);
        } else {
            break;
        }
    }
}

void Simulator::updateBlockedJobs() {
    int activeCount = memory.getJobCount(ACTIVE_QUEUE);
    for (int i = 0; i < activeCount; i++) {
        Job* job = memory.getJob(ACTIVE_QUEUE, i);
        if (job->getState() == BLOCKED) {
            job->passBlockedTime();
            if (job->getBlockedTime() == 0) {
                job->setState(READY);
                memory.updateFramesState(job->getID(), READY);
            }
        }
    }
}

void Simulator::suspendJob() {
    int activeCount = memory.getJobCount(ACTIVE_QUEUE);
    int blockedIndex = -1;
    for(int i = 0; i < activeCount; ++i){
        if(memory.getJob(ACTIVE_QUEUE, i)->getState() == BLOCKED) {
            blockedIndex = i;
            break;
        }
    }
    if(blockedIndex != -1) {
        Job* jobToSuspend = memory.getJob(ACTIVE_QUEUE, blockedIndex);
        jobToSuspend->setState(SUSPENDED);
        memory.freeFrameJobs(jobToSuspend->getID());
        ofstream file(SUSPENDED_FILE, ios::app);
        if(file.is_open()) {
            file << "ID: " << jobToSuspend->getID()
                << " | OPE: " << jobToSuspend->getOperation()
                << " | TAM: " << jobToSuspend->getSize() << "\n";
            file.close();
        }
        memory.moveJob(ACTIVE_QUEUE, SUSPENDED_QUEUE, blockedIndex);
    }
}

void Simulator::returnJob() {
    if(!memory.isEmpty(SUSPENDED_QUEUE)) {
        Job* returnJob = memory.getJob(SUSPENDED_QUEUE, 0);
        if(memory.getFreeFrames() >= returnJob->getPageCount()) {
            returnJob->setState(READY);
            memory.allocateJob(returnJob);
            memory.moveJob(SUSPENDED_QUEUE, ACTIVE_QUEUE, 0);
        }
    }
}

void Simulator::calculateFinalTimes(Job* job) {
    job->setCompletionTime(this->globalCounter);
    job->setServiceTime(job->getElapsedTime());
    job->setReturnTime(job->getCompletionTime() - job->getArrivalTime());
    job->setWaitingTime(job->getReturnTime() - job->getServiceTime());
}

string Simulator::formatResult(Job* job) {
    if (job->getState() == ERROR) return "ERROR";
    if (job->getState() != TERMINATED) return "N/A";
    stringstream ss;
    ss << fixed << setprecision(2) << job->getResult();
    return ss.str();
}

void Simulator::handleRunningKeys(char key, Job* currentJob) {
    switch (key) {
        case 'I':
            currentJob->setState(BLOCKED);
            currentJob->setBlockedTime(BLOCKED_TIME);
            memory.updateFramesState(currentJob->getID(), BLOCKED);
            memory.rotateActiveQueue();
            break;
        case 'E':
            currentJob->setState(ERROR);
            calculateFinalTimes(currentJob);
            memory.moveJob(ACTIVE_QUEUE, TERMINATED_LOG, 0);
            memory.freeFrameJobs(currentJob->getID());
            break;
        case 'N':
            generateJobs(1);
            loadJobsToMemory();
            break;
        case 'B':
            printBCPTable();
            break;
        case 'P':
            pauseSimulation();
            break;
        case 'T':
            showPageTable();
            break;
        case 'S':
            suspendJob();
            break;
        case 'R':
            returnJob();
            break;
    }
}

void Simulator::pauseSimulation() {
    bool paused = true;
    while (paused) {
        while (Console::keyPressed()) {
            if (toupper(Console::getKey()) == 'C') paused = false;
        }
        if (paused) Console::sleep(1);
    }
}

void Simulator::executeRunningJob(Job* currentJob) {
    if (currentJob->getResponseTime() == -1) {
        currentJob->setResponseTime(this->globalCounter - currentJob->getArrivalTime());
    }

    currentJob->setState(RUNNING);
    memory.updateFramesState(currentJob->getID(), RUNNING);
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
            handleRunningKeys(key, currentJob);
        }

        if (currentJob->getState() != RUNNING) break;

        if (currentJob->getElapsedTime() == currentJob->getEstimatedTime()) {
            currentJob->calculateResult();
            currentJob->setState(TERMINATED);
            calculateFinalTimes(currentJob);
            memory.moveJob(ACTIVE_QUEUE, TERMINATED_LOG, 0);
            memory.freeFrameJobs(currentJob->getID());
            break;
        }

        if (quantumCnt == this->quantum) {
            currentJob->setState(READY);
            memory.updateFramesState(currentJob->getID(), READY);
            memory.rotateActiveQueue();
            break;
        }
    }
}

void Simulator::executeIdleCPU() {
    render();
    Console::sleep(1);
    updateBlockedJobs();
    globalCounter++;

    if (Console::keyPressed()) {
        char key = toupper(Console::getKey());
        handleRunningKeys(key, nullptr);
    }
}

void Simulator::processActiveQueue() {
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
        executeRunningJob(memory.getJob(ACTIVE_QUEUE, 0));
    } else {
        executeIdleCPU();
    }
}

void Simulator::run() {
    Console::TerminalMode termMode;
    while (!memory.isEmpty(JOB_QUEUE) || !memory.isEmpty(ACTIVE_QUEUE) || !memory.isEmpty(SUSPENDED_QUEUE)) {
        loadJobsToMemory();

        if (!memory.isEmpty(ACTIVE_QUEUE)) {
            processActiveQueue();
        } else if(!memory.isEmpty(SUSPENDED_QUEUE)) {
            executeIdleCPU();
        }
    }

    render();
    Console::pause();
}

void Simulator::showPageTable() {
    Console::clearScreen();
    int totalWidth = 74;
    centerText("TABLA DE PAGINAS Y ESTADO DE MEMORIA", totalWidth);
    cout << "| Mco | Espacio | Proceso | Pag |  | Mco | Espacio | Proceso | Pag |" << endl;
    cout << string(totalWidth, '-') << endl;

    string freeFrames = "";

    for (int i = 0; i < 23; i++) {
        auto printFrameRow = [&](int idx) {
            Frame& f = memory.getFrame(idx); 
            cout << "| " << setw(3) << left << f.frameID << " | ";
            cout << setw(7) << left << (to_string(f.usedSpace) + "/5") << " | ";

            if (f.jobID == 0) {
                cout << setw(7) << left << "S.O." << " | " << setw(3) << left << "-" << " |";
            } else if (f.jobID == -1) {
                cout << setw(7) << left << "Libre" << " | " << setw(3) << left << "-" << " |";
                freeFrames += to_string(f.frameID) + " ";
            } else {
                cout << setw(7) << left << f.jobID << " | " << setw(3) << left << f.pageID << " |";
            }
        };

        printFrameRow(i);      
        cout << "  ";          
        printFrameRow(i + 23); 
        cout << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << "Marcos Libres: " << (freeFrames.empty() ? "Ninguno" : freeFrames) << endl;
    cout << "\nPresione 'C' para continuar la simulacion..." << endl;

    pauseSimulation();
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
    for (int i = 0; i < x; i++) cout << "-";
    cout << " " << text << " ";
    for (int i = 0; i < (width - x - totalLength); i++) cout << "-";
    cout << endl;
}

void Simulator::printRunningState() {
    Console::clearScreen();
    int width = W_ID + W_OPE + W_RES;

    int pendingJobs = memory.getJobCount(JOB_QUEUE);
    int suspendedJobs = memory.getJobCount(SUSPENDED_QUEUE);

    cout << "Quantum: " << this->quantum << " | Contador Global: " << globalCounter << endl;
    cout << "No. Procesos en cola de Nuevos: " << pendingJobs << endl;
    cout << "No. Procesos en estado Suspendido: " << suspendedJobs << endl;

    if (pendingJobs > 0) {
        Job* next = memory.getJob(JOB_QUEUE, 0);
        cout << "PROXIMO A ENTRAR -> ID: " << next->getID() << " | Tam: " << next->getSize() << endl;
    } else {
        cout << "PROXIMO A ENTRAR -> Ninguno" << endl;
    }

    if (suspendedJobs > 0) {
        Job* nextSuspended = memory.getJob(SUSPENDED_QUEUE, 0);
        cout << "PROXIMO A REGRESAR (Suspendido) -> ID: " << nextSuspended->getID() << " | Tam: " << nextSuspended->getSize() << endl;
    } else {
        cout << "PROXIMO A REGRESAR (Suspendido) -> Ninguno" << endl;
    }
    cout << endl;

    printMemoryMap(); 
    cout << endl;

    int activeJobCount = memory.getJobCount(ACTIVE_QUEUE);

    centerText("Proceso Actual", width);
    bool isCpuIdle = true;
    for (int i = 0; i < activeJobCount; i++) {
        Job* job = memory.getJob(ACTIVE_QUEUE, i);
        if (job->getState() == RUNNING) {
            cout << "ID: " << job->getID() << " | OPE: " << job->getOperation() << " | TME: " << job->getEstimatedTime() << " | TT: " << job->getElapsedTime() << endl;
            isCpuIdle = false; 
            break;
        }
    }
    if (isCpuIdle) cout << "[CPU en espera (IDLE)]" << endl;

    cout << endl;
    centerText("Terminados", width);
    cout << left << setw(W_ID) << "ID" << setw(W_OPE) << "Ope" << setw(W_RES) << "Res" << endl;
    int terminatedCount = memory.getJobCount(TERMINATED_LOG);
    for (int i = 0; i < terminatedCount; i++) {
        Job* job = memory.getJob(TERMINATED_LOG, i);
        cout << left << setw(W_ID) << job->getID() << setw(W_OPE) << job->getOperation() << setw(W_RES) << formatResult(job) << endl;
    }

    cout << "\n\"I\"-Interr, \"E\"-Error, \"P\"-Pausa, \"N\"-Nuevo, \"B\"-BCP, \"T\"-Tabla Paginas, \"S\"-Susp, \"R\"-Regresar" << endl;
}

void Simulator::printFinalState() {
    Console::clearScreen();
    int width = W_ID + W_OPE + W_RES + (W_TIME * 6);

    cout << "Todos los procesos han terminado." << endl << endl;
    centerText("Resumen Final", width);

    cout << left << setw(W_ID) << "ID" << setw(W_OPE) << "Operacion" << setw(W_RES) << "Resultado"
         << setw(W_TIME) << "Llegada" << setw(W_TIME) << "Fin" << setw(W_TIME) << "Retorno"
         << setw(W_TIME) << "Resp" << setw(W_TIME) << "Espera" << setw(W_TIME) << "Servicio" << endl;

    for (int i = 0; i < width; i++) cout << "-";
    cout << endl;

    int terminatedCount = memory.getJobCount(TERMINATED_LOG);
    for (int i = 0; i < terminatedCount; i++) {
        Job* job = memory.getJob(TERMINATED_LOG, i);

        cout << left << setw(W_ID) << job->getID() << setw(W_OPE) << job->getOperation() << setw(W_RES) << formatResult(job);
        cout << setw(W_TIME) << job->getArrivalTime() << setw(W_TIME) << job->getCompletionTime()
             << setw(W_TIME) << job->getReturnTime() << setw(W_TIME) << job->getResponseTime()
             << setw(W_TIME) << job->getWaitingTime() << setw(W_TIME) << job->getServiceTime() << endl;
    }

    cout << endl << "Contador final: " << globalCounter << endl << endl;
}

void Simulator::render() {
    if (!memory.isEmpty(JOB_QUEUE) || !memory.isEmpty(ACTIVE_QUEUE) || !memory.isEmpty(SUSPENDED_QUEUE)) {
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
            case NEW:        estadoStr = "NUEVO"; break;
            case READY:      estadoStr = "LISTO"; break;
            case RUNNING:    estadoStr = "EJECUCION"; break;
            case BLOCKED:    estadoStr = "BLOQ (" + to_string(job->getBlockedTime()) + ")"; break;
            case TERMINATED: estadoStr = "TERMINADO"; break;
            case ERROR:      estadoStr = "ERROR"; break;
            case SUSPENDED:  estadoStr = "SUSPENDIDO"; break;
        }
        cout << setw(14) << estadoStr << setw(W_OPE) << job->getOperation() << setw(W_RES) << formatResult(job);

        if (st == NEW) {
            cout << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A"
                 << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A"
                 << setw(W_TIME) << "N/A" << endl;
        } else {
            cout << setw(W_TIME) << job->getArrivalTime();

            if (st == TERMINATED || st == ERROR) {
                cout << setw(W_TIME) << job->getCompletionTime() << setw(W_TIME) << job->getReturnTime();
            } else {
                cout << setw(W_TIME) << "N/A" << setw(W_TIME) << "N/A";
            }

            int espera = (st == TERMINATED || st == ERROR)
                             ? job->getWaitingTime()
                             : (globalCounter - job->getArrivalTime() - job->getElapsedTime());
            cout << setw(W_TIME) << espera << setw(W_TIME) << job->getElapsedTime();

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
    for (int i = 0; i < memory.getJobCount(ACTIVE_QUEUE); i++) printJob(memory.getJob(ACTIVE_QUEUE, i));
    for (int i = 0; i < memory.getJobCount(SUSPENDED_QUEUE); i++) printJob(memory.getJob(SUSPENDED_QUEUE, i));
    for (int i = 0; i < memory.getJobCount(TERMINATED_LOG); i++) printJob(memory.getJob(TERMINATED_LOG, i));

    cout << endl << "Contador actual: " << globalCounter << endl;
    cout << "\nPresione 'C' para continuar..." << endl;

    pauseSimulation();
}

void Simulator::printMemoryMap() {
    int totalWidth = 91; 
    centerText("ESTADO DE LA MEMORIA (46 MARCOS)", totalWidth);
    
    cout << "| Mco | Proceso | Pag | Uso |  | Mco | Proceso | Pag | Uso |  | Mco | Proceso | Pag | Uso |" << endl;
    cout << string(totalWidth, '-') << endl;

    for (int i = 0; i < 16; i++) {
        auto printFrame = [&](int idx) {
            if (idx >= 46) {
                cout << "|     |         |     |     |"; 
                return;
            }
            
            Frame& f = memory.getFrame(idx); 
            cout << "| " << setw(3) << left << idx << " | ";
            
            if (f.jobID == 0) {
                cout << setw(7) << left << "S.O." << " | " << setw(3) << left << "-" << " | " << setw(3) << left << "5/5" << " |";
            } else if (f.jobID == -1) {
                cout << setw(7) << left << "Libre" << " | " << setw(3) << left << "-" << " | " << setw(3) << left << "0/5" << " |";
            } else {
                string etiqueta = (f.state == RUNNING) ? "E" : (f.state == BLOCKED) ? "B" : "L";
                string procStr = to_string(f.jobID) + "(" + etiqueta + ")";
                
                cout << setw(7) << left << procStr << " | " << setw(3) << left << f.pageID << " | " << setw(3) << left << (to_string(f.usedSpace) + "/5") << " |";
            }
        };

        printFrame(i);      cout << "  ";
        printFrame(i + 16); cout << "  ";
        printFrame(i + 32); cout << endl;
    }
    cout << string(totalWidth, '-') << endl;
}