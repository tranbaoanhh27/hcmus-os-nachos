// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

#define MAX_SIZE 100
// Read and write register
#define SYSCALL_RET(VALUE) kernel->machine->WriteRegister(2, VALUE)
// #define WRITE_REGISTER(REG, VALUE) kernel->machine->ReadRegister(REG, VALUE)
#define WRITE_REGISTER(REG, VALUE) kernel->machine->WriteRegister(REG, SysExec(VALUE))
#define READ_REGISTER(REG) kernel->machine->ReadRegister(REG)
#define GET_ARGUMENT(N) kernel->machine->ReadRegister(N + 3)

// Hàm lấy tên chương trình load into kernel space
char *user2System(int virtAddr, int limit = -1)
{
	int i = 0;
	int oneChar;
	char *buffer = nullptr;

	if (limit == -1)
		++limit;
	buffer = new char[limit + 1];
	if (buffer == nullptr)
		return buffer;

	memset(buffer, 0, limit + 1);

	for (; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		buffer[i] = (unsigned char)oneChar;
		if (oneChar == 0)
			break;
	}

	return buffer;
}

int system2User(char *buffer, int virtAddr, int limit = -1)
{
	if (limit < 0)
		return -1;
	if (limit == 0)
		return limit;

	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < limit && oneChar != 0);

	return i;
}

void increasePC()
{
	int get_arg = READ_REGISTER(PCReg);
	kernel->machine->WriteRegister(PrevPCReg, get_arg);
	get_arg = READ_REGISTER(NextPCReg);
	kernel->machine->WriteRegister(PCReg, get_arg);
	kernel->machine->WriteRegister(NextPCReg, get_arg + 4);
}

void solve_SC_Halt()
{
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
	SysHalt();
	ASSERTNOTREACHED();
}

void solve_SC_ThreadJoin(int type)
{
	DEBUG(dbgSys, "Not yet implemented syscall " << type << "\n");
	increasePC();
}

void solve_SC_Add()
{
	DEBUG(dbgSys, "Add " << GET_ARGUMENT(1) << " + " << GET_ARGUMENT(2) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */ (int)GET_ARGUMENT(1), /* int op2 */ (int)GET_ARGUMENT(2));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	/* increase program counter */
	increasePC();
}

void solve_SC_RandomNum()
{
	DEBUG(dbgSys, "System call RandomNum invoked!");
	int generatedRandomNum;
	generatedRandomNum = SysRandomNum();
	DEBUG(dbgSys, "RandomNum returning with " << generatedRandomNum << "\n");
	kernel->machine->WriteRegister(2, (int)generatedRandomNum);
	increasePC();
}

void solve_SC_ReadString()
{
	DEBUG(dbgSys, "Syscall ReadString invoked!");

	// Lấy tham số từ thanh ghi 4 và 5
	int virtAddr, length;
	virtAddr = GET_ARGUMENT(1);
	length = GET_ARGUMENT(2);
	DEBUG(dbgSys, "SC_ReadString: User space buffer address is " << virtAddr);
	DEBUG(dbgSys, "SC_ReadString: Number of characters to read: " << length);

	// Tạo kernel space buffer
	char *buffer;
	buffer = (char *)malloc(length + 1);
	if (buffer == NULL)
	{
		DEBUG(dbgSys, "SC_ReadString: ERROR: Failed to allocate memory for kernel space buffer");
		return;
	}

	// Đọc chuỗi từ console vào trong kernel space buffer
	SysReadString(buffer, length);

	DEBUG(dbgSys, "SC_ReadString: ReadString result: " << buffer << "\n");
	DEBUG(dbgSys, "SC_ReadString: ASCII Codes:");
	for (int i = 0; i < length + 1; i++)
		DEBUG(dbgSys, (int)buffer[i]);

	// Copy chuỗi từ kernel space buffer sang user space buffer
	system2User(buffer, virtAddr, length + 1);

	// Giải phóng bộ nhớ
	free(buffer);

	// Tăng program counter
	increasePC();
}

void solve_SC_PrintString()
{
	DEBUG(dbgSys, "Syscall PrintString invoked!");

	// Lấy tham số từ thanh ghi 4
	int virtAddr = GET_ARGUMENT(1);
	DEBUG(dbgSys, "SC_PrintString: User space buffer address is " << virtAddr);

	// Chuyển chuỗi từ user space sang kernel space
	char *buffer;
	buffer = nullptr;
	const int MAX_STRING = 2048;
	buffer = user2System(virtAddr, MAX_STRING);

	if (buffer == nullptr)
	{
		DEBUG(dbgSys, "SC_PrintString: Failed to copy User2System");
		increasePC();
		return;
	}

	DEBUG(dbgSys, "SC_PrintString: kernel space buffer content: " << buffer);

	// Write chuỗi ra console
	SysPrintString(buffer, MAX_STRING);

	// Giải phóng bộ nhớ
	delete[] buffer;

	// Tăng Program Counter
	increasePC();
}

void solve_SC_ReadNum()
{
	DEBUG(dbgSys, "ReadNum: execute the system call \n");
	int result = SysReadNum();

	DEBUG(dbgSys, "ReadNum: gia tri tra ve: " << result << "\n");
	kernel->machine->WriteRegister(2, (int)result);
	increasePC();
}

void solve_SC_PrintNum()
{
	DEBUG(dbgSys, "PrintNum: execute the system call \n");
	int character = kernel->machine->ReadRegister(4);
	SysPrintNum(character);
	increasePC();
}

void solve_SC_ReadChar()
{
	DEBUG(dbgSys, "ReadChar: execute the system call \n");
	char result = SysReadChar();
	kernel->machine->WriteRegister(2, (int)result);
	DEBUG(dbgSys, "ReadChar: ki tu tra ve: " << result << "\n");
	increasePC();
}


void solve_SC_PrintChar() {
	DEBUG(dbgSys, "PrintChar: execute the system call \n");
	char character = (char)kernel->machine->ReadRegister(4);
	SysPrintChar(character);
	increasePC();
}

void solve_SC_Create()
{
	DEBUG(dbgSys, "CreatFile: execute the system call \n");
	const int MAX_STRING = 2048;
	int virtAddr = kernel->machine->ReadRegister(4);
	char *fileName = nullptr;

	fileName = user2System(virtAddr, MAX_STRING);
	if (fileName == nullptr)
	{
		DEBUG(dbgSys, "CreatFile: cannot create a new file \n");
		kernel->machine->WriteRegister(2, -1);
		increasePC();
		return;
	}

	int result = SysCreate(fileName);
	if (result == -1)
	{
		DEBUG(dbgSys, "CreatFile: cannot create a new file \n");
	}

	kernel->machine->WriteRegister(2, result);

	delete[] fileName;
	fileName = nullptr;

	increasePC();
}

void solve_SC_Open()
{

	DEBUG(dbgSys, "OpenFile: execute the system call \n");
	const int MAX_STRING = 2048;
	int virtAddr = kernel->machine->ReadRegister(4);
	char *fileName = nullptr;
	int result = -1;

	fileName = user2System(virtAddr, MAX_STRING);
	if (fileName == nullptr)
	{
		DEBUG(dbgSys, "OpenFile: cannot read the file name \n");
		kernel->machine->WriteRegister(2, -1);
		increasePC();
		return;
	}

	int type = kernel->machine->ReadRegister(5);
	kernel->machine->WriteRegister(2, SysOpen(fileName, type));

	delete fileName;
	fileName = nullptr;
	increasePC();
}

void solve_SC_Close()
{

	DEBUG(dbgSys, "CloseFile: execute the system call \n");
	int fileID = kernel->machine->ReadRegister(4);
	int result = SysClose(fileID);
	if (result == -1)
	{
		DEBUG(dbgSys, "CloseFile: cannot close the file \n");
	}
	kernel->machine->WriteRegister(2, result);
	increasePC();
}

void solve_SC_Write()
{
	DEBUG(dbgSys, "WriteFile: execute the system call \n");
	int virtAddress = kernel->machine->ReadRegister(4);
	int charCount = kernel->machine->ReadRegister(5);
	int id = kernel->machine->ReadRegister(6);
	char *buffer = user2System(virtAddress, charCount);
	if (buffer == nullptr)
	{
		DEBUG(dbgSys, "WriteFile: cannot write in file \n");
		kernel->machine->WriteRegister(2, -1);
		increasePC();
		return;
	}

	int result = SysWrite(buffer, charCount, id);
	if (result == -1)
	{
		DEBUG(dbgSys, "WriteFile: cannot write in file \n");
	}
	delete[] buffer;
	buffer = nullptr;
	kernel->machine->WriteRegister(2, result);
	increasePC();
}

void solve_SC_Read()
{
	DEBUG(dbgSys, "ReadFile: execute the system call \n");
	int virtAddress = kernel->machine->ReadRegister(4);
	int charCount = kernel->machine->ReadRegister(5);
	int id = kernel->machine->ReadRegister(6);
	char *buffer = user2System(virtAddress, charCount);
	if (buffer == nullptr)
	{
		DEBUG(dbgSys, "ReadFile: cannot read in file\n");
		kernel->machine->WriteRegister(2, -1);
		increasePC();
		return;
	}

	int result = SysRead(buffer, charCount, id);
	if (result == -1)
	{
		DEBUG(dbgSys, "ReadFile: cannot read in file\n");
	}
	system2User(buffer, virtAddress, charCount);
	kernel->machine->WriteRegister(2, result);

	delete[] buffer;
	buffer = nullptr;
	increasePC();
}

void solve_SC_Seek()
{
	DEBUG(dbgSys, "Seek: execute the system call \n");
	int pos = kernel->machine->ReadRegister(4);
	int id = kernel->machine->ReadRegister(5);
	int result = SysSeek(pos, id);
	if (result == -1)
	{
		DEBUG(dbgSys, "Seek: cannot seek in file\n");
	}
	kernel->machine->WriteRegister(2, result);
	increasePC();
}

void solve_SC_Remove()
{
	DEBUG(dbgSys, "RemoveFile: execute the system call \n");
	const int MAX_STRING = 2048;
	int virtAddress = kernel->machine->ReadRegister(4);
	char *fileName = user2System(virtAddress, MAX_STRING);
	if (fileName == nullptr)
	{
		DEBUG(dbgSys, "RemoveFile: cannot remove file\n");
		kernel->machine->WriteRegister(2, -1);
		increasePC();
		return;
	}

	int result = SysRemove(fileName);
	if (result == -1)
	{
		DEBUG(dbgSys, "RemoveFile: cannot remove file\n");
	}
	kernel->machine->WriteRegister(2, result);
	increasePC();
}

// ***********************************************************************************

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);
	DEBUG(dbgSys, "Received Exception: " << which << ", type: " << type << "\n");

	switch (which)
	{
	case NoException:
		kernel->interrupt->setStatus(SystemMode);
		DEBUG(dbgSys, "\nSwitch to SystemMode\n");
		break;

	case PageFaultException:
		DEBUG('a', "\nPageFaultException: No valid translation found.\n");
		printf("\nPageFaultException: No valid translation found.\n");
		kernel->interrupt->Halt();
		break;

	case ReadOnlyException:
		DEBUG('a', "\nReadOnlyException: Write attempted to page marked read-only.\n");
		printf("\nReadOnlyException: Write attempted to page marked read-only.\n");
		kernel->interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG('a', "\nBusErrorException: Translation resulted invalid physical address.\n");
		printf("\nBusErrorException: Translation resulted invalid physical address.\n");
		kernel->interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG('a', "\nAddressErrorException: Unaligned reference or one that was beyond the end of the address space.\n");
		printf("\nAddressErrorException: Unaligned reference or one that was beyond the end of the address space.\n");
		kernel->interrupt->Halt();
		break;

	case OverflowException:
		DEBUG('a', "\nOverflowException: Integer overflow in add or sub.\n");
		printf("\nOverflowException: Integer overflow in add or sub.\n");
		kernel->interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\nIllegalInstrException: Unimplemented or reserved instr.\n");
		printf("\nIllegalInstrException: Unimplemented or reserved instr.\n");
		kernel->interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\nNumExceptionTypes: Number exception types.\n");
		printf("\nNumExceptionTypes: Number exception types.\n");
		kernel->interrupt->Halt();
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			solve_SC_Halt();
			return;
			break;

		case SC_ThreadJoin:
			solve_SC_ThreadJoin(type);
			return;
			break;

		case SC_Add:
			solve_SC_Add();
			return;
			break;

		case SC_RandomNum:
			solve_SC_RandomNum();
			return;
			break;

		case SC_ReadString:
			solve_SC_ReadString();
			return;
			break;

		case SC_PrintString:
			solve_SC_PrintString();
			return;
			break;

		case SC_ReadNum:
			solve_SC_ReadNum();
			return;
			break;

		case SC_PrintNum:
			solve_SC_PrintNum();
			return;
			break;

		case SC_ReadChar:
			solve_SC_ReadChar();
			return;
			break;

		case SC_PrintChar:
			solve_SC_PrintChar();
			return;
			break;

		case SC_Open:
			solve_SC_Open();
			return;
			break;

		case SC_Write:
			solve_SC_Write();
			return;
			break;
		
		case SC_Read:
			solve_SC_Read();
			return;
			break;

		case SC_Seek:
			solve_SC_Seek();
			return;
			break;

		case SC_Remove:
			solve_SC_Remove();
			return;
			break;

		case SC_Create:
			solve_SC_Create();
			return;
			break;

		case SC_Close:
			solve_SC_Close();
			return;
			break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}