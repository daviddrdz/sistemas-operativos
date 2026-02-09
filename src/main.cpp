#include <iostream>

#include "JobManager.h"
#include "Simulator.h"

using namespace std;

int main() {
    int numJobs;

    JobManager jobManager;
    Simulator simulator;

    cout << "Introduzca la cantidad de procesos: ";
    cin >> numJobs;
    cin.get();

    simulator.createBatches(numJobs, &jobManager);
    simulator.processBatches();

    return 0;
}