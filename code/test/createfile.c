#include "syscall.h"

void inputFilename(char *buffer) {
    int filenameLength;
    do
    {
        PrintString("Enter length of filename (maximum 256 characters): ");
        filenameLength = ReadNum();
        if (!(filenameLength > 0 && filenameLength < 257))
            PrintString("Please enter filename's length value in range [1, 256]\n");
    } while (!(filenameLength > 0 && filenameLength < 257));

    // Get filename from keyboard
    PrintString("Enter filename (exactly ");
    PrintNum(filenameLength);
    PrintString(" characters): ");
    ReadString(buffer, filenameLength);
}

void main()
{
    char filename[256];
    inputFilename(filename);
    if (Create(filename) == 0)
        PrintString("\nSUCCESS: Successfully created file ");
    else
        PrintString("\nFAILED: Can't create file ");
    PrintString(filename);
    PrintChar('\n');

    Halt();
}