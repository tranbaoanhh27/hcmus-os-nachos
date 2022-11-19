#include "syscall.h"

int
main() {
    char buffer1[20];
    int file = Open("abc.txt", 1);
    PrintNum(file);
    // PrintString("\n");
    // Read(buffer1, 2, file);
    // PrintString(buffer1);
    Read(buffer1, 19, file);
    PrintString(buffer1);
    PrintString("\n");
    Close(file);
    PrintNum(Write("20120028", 8, file));
    PrintString("\n");

    // char buffer[11]="";
    // int fileID = Open("an.txt", 1);
    // Seek(1, fileID);
    // Read(buffer, 10, fileID);
    // Close(fileID);
    // PrintString(buffer2);
    // PrintString("\n");

    Halt();
}