// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
// #include "syscall.h"
// #include "filetable.h"

#define MAX_PROCESS 1
#define MAX_FILE 20
#define CONSOLE_IN 0
#define CONSOLE_OUT 1
#define MODE_READWRITE 0
#define MODE_READ 1

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem
{
private:
    OpenFile ***FileTable;
    int **id;

public:
    FileSystem()
    {
        FileTable = new  OpenFile**[MAX_PROCESS];
        id = new int *[MAX_PROCESS];
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            FileTable[i] = new OpenFile*[MAX_FILE];
            id[i] = new int[MAX_FILE];
            id[i][0] = CONSOLE_IN;
            id[i][1] = CONSOLE_OUT;
        }
    }

    ~FileSystem()
    {
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            if (FileTable[i] != nullptr)
            {
                delete FileTable[i];
                FileTable[i] = nullptr;
            }

            if (id[i] != nullptr)
            {
                delete[] id[i];
                id[i] = nullptr;
            }
        }

        delete[] FileTable;
        FileTable = nullptr;
        delete[] id;
        id = nullptr;
    }

    bool Create(char *name)
    {
        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1)
            return FALSE;
        Close(fileDescriptor);
        return TRUE;
    }

    int OpenMode(char *name, int type = 0)
    {
        int fileDescriptor;

        if (type == 0)
        {
            fileDescriptor = OpenForRead(name, FALSE);
        }
        else if (type == 1)
        {
            fileDescriptor = OpenForReadWrite(name, FALSE);
        }

        bool k = false;
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (FileTable[i][j] == nullptr)
                {
                    FileTable[i][j] = new OpenFile(fileDescriptor);
                    id[i][j] = fileDescriptor;
                    k = true;
                    break;
                }
            }
        }

        if (k)
        {
            return fileDescriptor;
        }
        else
        {
            return -1;
        }
    }

    int Write(char *buffer, int size, int fileID)
    {
        if (fileID < 0)
            return -1;
        
        OpenFile *file = nullptr;

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {
                    file = FileTable[i][j];
                    break;
                }
            }
        }

        if (file != nullptr) {
            int result = file->Write(buffer, size);
            file = nullptr;
            return result;
        }
        else {
            return -1;
        }
    }

    int Read(char *buffer, int size, int fileID)
    {
        if (fileID < 0)
            return -1;
        
        OpenFile *file = nullptr;

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {
                    file = FileTable[i][j];
                    break;
                }
            }
        }

        if (file != nullptr) {
            int result = file->Read(buffer, size);
            file = nullptr;
            return result;
        }
        else {
            return -1;
        }
    }

    int Seek(int pos, int fileID)
    {
        if (fileID < 0)
            return -1;
        
        OpenFile *file = nullptr;

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {
                    file = FileTable[i][j];
                    break;
                }
            }
        }

        if (file != nullptr) {
            if (pos < -1) {
                return -1;
            }
            else if (pos == -1) {
                pos = file->Length();
            }

            int result = file->Seek(pos);
            file = nullptr;
            return result;
        }
        else {
            return -1;
        }
    }

    int Close(int fileID) {       
        if (fileID < 0) {
            return -1;
        } 

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {

                    delete FileTable[i][j];
                    FileTable[i][j] = nullptr;
                    id[i][j] = NULL;
                    Close(fileID);
                    return 0;
                    break;
                }
            }
        } 
    }

    int Remove(char *name) { 

        int fileDes1 = OpenForReadWrite(name, FALSE);
        int fileDes2 = OpenForRead(name, FALSE);

        OpenFile *file = nullptr;

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileDes1 || id[i][j] == fileDes2)
                {
                    file = FileTable[i][j];
                    break;
                }
            }
        }

        if (file == nullptr) {
            bool result = Unlink(name);
            if (result == 0) {
                return  -1;
            }
            else {
                return result;
            }    
        }
        else return -1;
    }

    OpenFile *
    Open(char *name)
    {
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        if (fileDescriptor == -1)
            return NULL;
        return new OpenFile(fileDescriptor);
    }
};

// class FileSystem {
//    public:
//     FileTable **fileTable;

//     FileSystem() {
//         fileTable = new FileTable *[MAX_PROCESS];
//         for (int i = 0; i < MAX_PROCESS; i++) {
//             fileTable[i] = new FileTable;
//         }
//     }

//     ~FileSystem() {
//         for (int i = 0; i < MAX_PROCESS; i++) {
//             delete fileTable[i];
//         }
//         delete[] fileTable;
//     }

//     bool Create(char *name) {
//         int fileDescriptor = OpenForWrite(name);

//         if (fileDescriptor == -1) return FALSE;
//         Close(fileDescriptor);
//         return TRUE;
//     }

//     // OpenFile *Open(char *name);

//     // int FileTableIndex();

//     void Renew(int id) {
//         for (int i = 0; i < FILE_MAX; i++) {
//             fileTable[id]->Remove(i);
//         }
//     }

//     int Open(char *name, int openMode = 1) {
//         return fileTable[0]->Insert(name, openMode);
//     }

//     // int Close(int id) { return fileTable[FileTableIndex()]->Remove(id); }

//     // int Read(char *buffer, int charCount, int id) {
//     //     return fileTable[FileTableIndex()]->Read(buffer, charCount, id);
//     // }

//     int Write(char *buffer, int charCount, int id) {
//         return fileTable[0]->Write(buffer, charCount, id);
//     }

//     // int Seek(int position, int id) {
//     //     return fileTable[FileTableIndex()]->Seek(position, id);
//     // }

//     bool Remove(char *name) { return Unlink(name) == 0; }
// };

#else // FILESYS
class FileSystem
{
public:
    FileSystem(bool format); // Initialize the file system.
                             // Must be called *after* "synchDisk"
                             // has been initialized.
                             // If "format", there is nothing on
                             // the disk, so initialize the directory
                             // and the bitmap of free blocks.

    bool Create(char *name, int initialSize);
    // Create a file (UNIX creat)

    OpenFile *Open(char *name); // Open a file (UNIX open)

    bool Remove(char *name); // Delete a file (UNIX unlink)

    void List(); // List all the files in the file system

    void Print(); // List all the files and their contents

private:
    OpenFile *freeMapFile;   // Bit map of free disk blocks,
                             // represented as a file
    OpenFile *directoryFile; // "Root" directory -- list of
                             // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
