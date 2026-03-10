#include "ConsoleUtils.hpp"

#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

namespace Console {

#ifndef _WIN32
static termios original_termios;  // Guarda la configuración normal de Linux
#endif

// Constructor: Se ejecuta al INICIAR la simulación (Apaga el eco)
TerminalMode::TerminalMode() {
#ifndef _WIN32
    tcgetattr(STDIN_FILENO, &original_termios);
    termios newt = original_termios;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
}

// Destructor: Se ejecuta al TERMINAR la simulación (Enciende el eco)
TerminalMode::~TerminalMode() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void sleep(int sec) { this_thread::sleep_for(chrono::seconds(sec)); }

void pause() {
    cout << "Press enter to continue . . .";
    cin.get();
}

bool keyPressed() {
#ifdef _WIN32
    return _kbhit();
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return (result > 0);
#endif
}

char getKey() {
#ifdef _WIN32
    return _getch();
#else
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
#endif
}

}  // namespace Console
