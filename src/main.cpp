#include <iostream>

#include "Simulator.hpp"

using namespace std;

int main() {
    int numJobs;

    Simulator simulator;

    cout << "Introduzca la cantidad de procesos: ";
    cin >> numJobs;
    cin.get();

    simulator.createBatches(numJobs);
    simulator.processBatches();

    return 0;
}