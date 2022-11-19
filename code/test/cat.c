
#include "syscall.h"

#define READONLY 1

int getFileLength(int fileID)
{
    return Seek(-1, fileID) + 1;
}

void inputFilename(char *buffer)
{
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
    int fileID;

    inputFilename(filename);

    // Try opening file
    fileID = Open(filename, READONLY);

    // If can't open file, show error notification
    if (fileID == -1)
    {
        PrintString("FAILED: Can't open file ");
        PrintString(filename);
    }

    // If open file successfully, try reading file
    else
    {
        int fileLength = getFileLength(fileID);
        char buffer[fileLength + 1];
        
        if (Read(buffer, fileLength + 1, fileID) == -1)
        {
            // If can't read file, show error notification
            PrintString("FAILED: Can't read file ");
            PrintString(filename);
        }

        // If read file's content successfully, show it
        else
        {
            PrintString("Content of file ");
            PrintString(filename);
            PrintString(":\n");
            PrintString(buffer);
        }

        // Always remembers to close the opened file.
        Close(fileID);
    }

    PrintChar('\n');

    // Halt machine!
    Halt();
}