#include "syscall.h"

int
main() {
    char buffer1[20];
    int file = Open("abc.txt", 1);
    PrintString("fileID: ");
    PrintNum(file);
    PrintString("\n");
    Read(buffer1, 19, file);
    PrintString("Doc tu file: ");
    PrintString(buffer1);
    PrintString("\n");
    Seek(1, file);
    Read(buffer1, 19, file);
    PrintString(buffer1);
    PrintString("\n");
    Close(file);

    file = Open("an.txt", 1);
    PrintString("fileID: ");
    PrintNum(file);
    PrintString("\n");
    Read(buffer1, 19, file);
    PrintString("Doc tu file: ");
    PrintString(buffer1);
    PrintString("\n");
    Close(file);

    Halt();
}