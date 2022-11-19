#include "syscall.h"

void
main() {
    char buffer[100];
    // int file = Open("abc.txt", 0);
    PrintNum(Remove("abc.txt"));
    // PrintNum(file);
    // Read(buffer, 20, file);
    // PrintString(buffer);
    // Seek(1, file);
    // Read(buffer, 20, file);
    // PrintString(buffer);
    // PrintNum(Write("20120028", 9, file));
    // Close(file);
    // file = Open("abc.txt", 0);
    // PrintNum(file);
    // file = Open("abc.txt", 0);
    // PrintNum(file);
    // Read(buffer, 20, file);
    // PrintString(buffer);
    // Write("20120028", 8, file);

    
    Halt();
}