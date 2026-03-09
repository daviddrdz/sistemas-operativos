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

bool keyPressed() {
#ifdef _WIN32
    return _kbhit();
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    // select revisa si hay bytes esperando sin sacarlos del buffer
    int result = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return (result > 0);
#endif
}

char getKey() {
#ifdef _WIN32
    return _getch();
#else
    termios oldt, newt;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Lee el byte real de forma segura
    read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
#endif
}

}  // namespace Console