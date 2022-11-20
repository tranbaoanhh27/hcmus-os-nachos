#include "syscall.h"

#define READWRITE 0
#define READONLY 1

int getFileLength(int fileID)
{
    int value = Seek(-1, fileID);
    Seek(0, fileID);
    return value;
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
    char sourceFilename[256];
    int sourceFileId;
    char destFilename[256];
    int destFileId;

    PrintString("Enter source file name:\n");
    inputFilename(sourceFilename);
    PrintString("Enter destination file name:\n");
    inputFilename(destFilename);

    if (Create(destFilename) == -1)
    {
        PrintString("FAILED: Can't create destination file!\n");
        return;
    }

    sourceFileId = Open(sourceFilename, READONLY);
    destFileId = Open(destFilename, READWRITE);

    if (sourceFileId == -1 || destFileId == -1) {
        PrintString("FAILED: Can't open source file or destination file!\n");
        if (sourceFileId != -1) Close(sourceFileId);
        if (destFileId != -1) Close(destFileId);
    }
    
    else
    {
        int sourceFileLength = getFileLength(sourceFileId);
        char sourceBuffer[sourceFileLength + 1];
        if (Read(sourceBuffer, sourceFileLength, sourceFileId) == -1)
            PrintString("FAILED: Can't read source file!\n");
        else
        {
            sourceBuffer[sourceFileLength] = '\0';
            if (Write(sourceBuffer, sourceFileLength, destFileId) == -1)
                PrintString("FAILED: Can't write to destination file!\n");
            else
                PrintString("SUCCESS: Copy file completed!\n");
        }

        Close(sourceFileId);
        Close(destFileId);
    }
}