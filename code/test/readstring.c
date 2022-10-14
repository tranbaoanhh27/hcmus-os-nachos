#include "syscall.h"

int main() {
    char buffer[11];
    ReadString(buffer, 8);
    PrintString(buffer);
    // Halt();
}