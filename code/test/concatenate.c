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

void main() {
    char filename1[256];
    char filename2[256];
    int fileId1;
    int fileId2;

    PrintString("Enter first file name:\n");
    inputFilename(filename1);
    PrintString("Enter second file name:\n");
    inputFilename(filename2);

    fileId1 = Open(filename1, READWRITE);
    fileId2 = Open(filename2, READONLY);

    if (fileId1 == -1 || fileId2 == -1) {
        PrintString("FAILED: Can't open file 1 or file 2\n");
        if (fileId1 != -1) Close(fileId1);
        if (fileId2 != -1) Close(fileId2);
    }
    else {
        int file2Length = getFileLength(fileId2);
        char buffer[file2Length + 1];

        if (Read(buffer, file2Length, fileId2) == -1)
            PrintString("FAILED: Can't read file 2!\n");
        else
        {
            Seek(-1, fileId1);
            buffer[file2Length] = '\0';
            if (Write(buffer, file2Length, fileId1) == -1)
                PrintString("FAILED: Can't write to file 1!\n");
            else
                PrintString("SUCCESS: Concatenated file 2 to file 1!\n");
        }

        Close(fileId1);
        Close(fileId2);
    }
}