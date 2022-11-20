#include "syscall.h"

int main() {
    char buffer[11];
    PrintString("Enter string of length 10: ");
    ReadString(buffer, 10);
    PrintString(buffer);
    PrintChar('\n');
    Halt();
}