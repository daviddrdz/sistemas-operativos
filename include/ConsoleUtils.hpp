#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

namespace Console {

class TerminalMode {
   public:
    TerminalMode();
    ~TerminalMode();
};

void clearScreen();
void sleep(int sec);
void pause();
bool keyPressed();
char getKey();

}  // namespace Console

#endif  // CONSOLE_UTILS_H
