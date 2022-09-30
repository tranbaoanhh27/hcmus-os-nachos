#include "syscall.h"

int main() {
    char buffer[11];
    ReadString(buffer, 10);
    PrintString(buffer);
    Halt();
}