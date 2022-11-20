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
#include <stdio.h>

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
    int **openMode;
    char ***name;

public:
    FileSystem()
    {
        // khoi tao bang
        FileTable = new OpenFile **[MAX_PROCESS];
        id = new int *[MAX_PROCESS];
        openMode = new int *[MAX_PROCESS];
        name = new char **[MAX_PROCESS];

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            FileTable[i] = new OpenFile *[MAX_FILE];
            id[i] = new int[MAX_FILE];

            // luu y 2 phan tu dau cua mang
            id[i][CONSOLE_IN] = CONSOLE_IN;
            id[i][CONSOLE_OUT] = CONSOLE_OUT;

            openMode[i] = new int[MAX_FILE];
            name[i] = new char *[MAX_FILE];
        }
    }

    ~FileSystem()
    {
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 0; j < MAX_FILE; j++)
            {
                if (FileTable[i][j] != nullptr)
                {
                    delete FileTable[i][j];
                    FileTable[i][j] = nullptr;
                }

                if (name[i][j] != nullptr)
                {
                    delete name[i][j];
                    name[i][j] = nullptr;
                }
            }

            if (FileTable[i] != nullptr) {
                delete FileTable[i];
                FileTable[i] = nullptr;
            }

            if (name[i] != nullptr) {
                delete name[i];
                name[i] = nullptr;
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
        delete[] name;
        name = nullptr;
    }

    /*
    kiem tra trong bang co file da cho co mo theo MODE_READ
    */
    bool checkFileModeRead(char *fileName)
    {
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (name[i][j] != nullptr && strcmp(fileName, name[i][j]) == 0 && openMode[i][j] == MODE_READ)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /*
    kiem tra trong bang co file da cho co mo theo MODE_READWRITE
    */
    bool checkFileModeReadWrite(char *fileName)
    {
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (name[i][j] != nullptr && strcmp(fileName, name[i][j]) == 0 && openMode[i][j] == MODE_READWRITE)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /*
    Tao file theo voi ten cho truoc
    */
    int Create(char *name)
    {
        if (strlen(name) == 0) {
            return -1;
        }

        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor < 0)
            return -1;
        Close(fileDescriptor);
        return 0;
    }

    /*
    mo file voi ten va OPEN_MODE cho truoc
    */
    int OpenMode(char *fileName, int type = 0)
    {
        if (strlen(fileName) == 0) {
            return -1;
        }

        int fileDescriptor;
        
        // mo them MODE duoc chi dinh san
        // luu y mot MODE voi moi File duoc mo duy nhat 1 lan
        // (readonly va readwrite) -> moi file toi da mo 2 mode
        if (type == MODE_READ)
        {
            // kiem tra file da mo MODE_READ
            if (checkFileModeRead(fileName)) {
                return -1;
            }
            else fileDescriptor = OpenForRead(fileName, FALSE);
        }
        else if (type == MODE_READWRITE)
        {
            // kiem tra file da mo MODE_READWRITE
            if (checkFileModeReadWrite(fileName)) {
                return -1;
            }
            else fileDescriptor = OpenForReadWrite(fileName, FALSE);
        }

        // them file vao bang
        bool k = false;
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (FileTable[i][j] == nullptr)
                {
                    // tao OpenFIle, them file Descriptor
                    // them ten file, mode
                    FileTable[i][j] = new OpenFile(fileDescriptor);
                    id[i][j] = fileDescriptor;
                    openMode[i][j] = type;
                    int len = strlen(fileName);
                    name[i][j] = new char[len];
                    int index = 0;
                    while (index != len)
                    {
                        name[i][j][index++] = fileName[index];
                    }
                    k = true;
                    break;
                }
            }
        }

        // neu con cho trong thi giu doi tuong OpenFile chua file can mo
        if (k)
        {
            return fileDescriptor;
        }
        else // neu khong con cho trong thi dong file vua mo
        {
            Close(fileDescriptor);
            return -1;
        }
    }

    int Write(char *buffer, int size, int fileID)
    {
        // file descriptor khong am
        if (fileID < 0)
            return -1;

        // do dai chuoi khong dung voi du lieu cho truoc
        if (strlen(buffer) != size) {
            return -1;
        }

        // tim kiem vi tri chua file
        OpenFile *file = nullptr;

        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {
                    file = FileTable[i][j];
                    if (openMode[i][j] != MODE_READWRITE) {
                        return -1;
                    }
                    break;
                }
            }
        }

        // tim thay file
        if (file != nullptr)
        {
            // goi ham thuc hien
            int result = file->Write(buffer, size);
            file = nullptr;
            return result;
        }
        // khong tim thay file
        else
        {
            return -1;
        }
    }

    int Read(char *buffer, int size, int fileID)
    {
        // file descriptor khong am
        if (fileID < 0)
            return -1;

        if (strlen(buffer) != size) {
            return -1;
        }

        // tim kiem vi tri cua file
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
        
        // neu tim thay
        if (file != nullptr)
        {
            int result = file->Read(buffer, size);
            file = nullptr;
            return result;
        }
        // khong tim thay
        else
        {
            return -1;
        }
    }

    int Seek(int pos, int fileID)
    {
        // file decriptor khong am
        if (fileID < 0)
            return -1;

        // tim kiem vi tri
        // va tien hanh them OpenFile moi vao bang
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

        // neu file tim thay
        if (file != nullptr)
        {
            // neu vi am va khac -1 (quy dinh -1 la cuoi file)
            if (pos < -1)
            {
                return -1;
            }
            // neu pos = -1 -> cuoi file
            else if (pos == -1)
            {
                pos = file->Length();
            }
            
            // thuc hien va tra ket qua
            int result = file->Seek(pos);
            file = nullptr;
            return result;
        }
        else // file khong tim thay
        {
            return -1;
        }
    }

    int Close(int fileID)
    {
        // kiem tra file descriptor
        if (fileID < 0)
        {
            return -1;
        }


        // tim kiem vi tri chua fileID 
        // xoa va lam rong
        for (int i = 0; i < MAX_PROCESS; i++)
        {
            for (int j = 2; j < MAX_FILE; j++)
            {
                if (id[i][j] == fileID)
                {

                    delete FileTable[i][j];
                    FileTable[i][j] = nullptr;
                    id[i][j] = NULL;
                    delete name[i][j];
                    name[i][j] = nullptr;
                    openMode[i][j] = NULL;
                    Close(fileID);
                    return 0;
                    break;
                }
            }
        }

        return -1;
    }

    int Remove(char *fileName)
    {
        if (strlen(fileName) == 0) {
            return -1;
        }

        // kiem tra neu file khong duoc mo
        if (checkFileModeRead(fileName) == false && checkFileModeReadWrite(fileName) == false) {
            // xoa file
            bool result = Unlink(fileName);
            if (result == 0)
            {
                return -1;
            }
            else
            {
                return result;
            }
        }
        return -1;
    }

    OpenFile *
    Open(char *fileName)
    {
        int fileDescriptor = OpenForReadWrite(fileName, FALSE);

        if (fileDescriptor == -1)
            return NULL;
        return new OpenFile(fileDescriptor);
    }
};

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
