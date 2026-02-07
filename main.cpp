#include <iostream>

#include "Job.h"
#include "Simulator.h"

using namespace std;

int main() {
    int numJobs;

    Simulator simulator;

    cout << "Introduzca la cantidad de procesos: ";
    cin >> numJobs;
    cin.get();

    simulator.captureJobs(numJobs);

    return 0;
}