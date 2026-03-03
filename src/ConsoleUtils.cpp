#include "ConsoleUtils.hpp"

#ifdef _WIN32
#include <conio.h>
#else
#include <fcntl.h>
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
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);  // guardar configuración actual
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // modo no canónico y sin eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // restaurar
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
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

    read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
#endif
}

}  // namespace Console