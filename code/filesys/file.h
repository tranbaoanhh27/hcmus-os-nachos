// #ifndef FILETABLE_H
// #define FILETABLE_H
// #include "openfile.h"
// #include "sysdep.h"
// #include "syscall.h"
// #include <stdio.h>
// #include <string.h>

// #define FILE_MAX 20
// #define CONSOLE_IN 0
// #define CONSOLE_OUT 1
// #define MODE_READ 0
// #define MODE_READWRITE 1
// #define Mode_WRITE 2

// class File
// {
// private:
//     // save list of openFile using list of pointer
//     // note that element 0 and 1 for console in/out
//     // fileID saveing id of each file
//     OpenFile* file;
//     int fileDescriptor;
//     int openMode;
//     char *name;

// public:
//     File(int fileID = 0, int mode = 0, char *fileName) {
//         file = new OpenFile(fileID);
//         fileDescriptor = openMode;
//         openMode = mode;
//         int len = strlen(fileName);
//         name = new char[len];
//         int index = 0;
//         while (index != len) {
//             name[index] = fileName[index];
//         }
//     }

//     ~File() {
//         if (file != nullptr) {
//             delete file;
//             file = nullptr;
//         }
//         if (name != nullptr) {
//             delete name;
//             name = nullptr;
//         }
//     }

//     bool checkFile

//     int Add(char *fileName, int mode)
//     {
//         int fileDescriptor = -1;
//         int freeIndex = -1;
//         // find the index of free
//         for (int i = 2; i < FILE_MAX; i++)
//         {
//             if (openFile[i] == nullptr)
//             {
//                 freeIndex = i;
//                 break;
//             }
//         }

//         // check the space of list
//         if (freeIndex == -1)
//             return -1;

//         if (mode == MODE_READ)
//         {
//             fileDescriptor = OpenForRead(fileName, false);
//         }

//         if (mode == MODE_READWRITE)
//         {
//             fileDescriptor = OpenForReadWrite(fileName, false);
//         }

//         if (fileDescriptor == -1)
//             return -1;

//         openFile[freeIndex] = new OpenFile(fileDescriptor);
//         openMode[freeIndex] = mode;

//         return freeIndex;
//     }

//     int Remove(int index)
//     {
//         if (index < 2 || index >= FILE_MAX)
//             return -1;
//         if (openFile[index])
//         {
//             delete openFile[index];
//             openFile[index] = nullptr;
//             openMode = NULL;
//         }
//         return -1;
//     }

//     int Write(char *buffer, int charCount, OpenFileId id)
//     {
//         if (id < 2 || id >= FILE_MAX)
//             return -1;
//         if (buffer != nullptr || openFile[id] == nullptr || openMode[id] != MODE_READWRITE)
//         {
//             return -1;
//         }

//         return openFile[id]->Write(buffer, charCount);
//     }

//     int Seek(int pos, OpenFileId id)
//     {
//         if (pos < -1 || id < 2)
//             return -1;
//         int fileLength = openFile[id]->Length();
//         if (pos >= openFile[id]->Length())
//             return -1;
//         if (pos == -1)
//             pos = fileLength; // note: cuoi file -> ko doc doc nua
//         openFile[id]->Seek(pos);
//         return pos;
//     }
// };
// #endif