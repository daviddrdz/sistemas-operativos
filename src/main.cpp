#include <iostream>

#include "Simulator.hpp"

using namespace std;

int main() {
    Simulator simulator;

    simulator.askData();
    simulator.run();

    return 0;
}