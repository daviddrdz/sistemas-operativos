#include "ConsoleUtils.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

namespace Console {
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void sleep(int sec) { this_thread::sleep_for(chrono::seconds(sec)); }

void pause() {
    cout << "Press enter to continue . . .";
    cin.get();
}
}  // namespace Console