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
#include "machine.h"
#include <malloc.h>
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

/*
 * Hàm tăng giá trị thanh ghi PC.
 * Đầu vào: không có.
 * Đầu ra: không có.
 */
void IncreasePC() {
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

/*
 * Hàm sao chép chuỗi bytes từ user space sang kernel space.
 * Đầu vào:
 * - int virtAddr: Địa chỉ vùng nhớ user space.
 * - int limit: Số bytes tối đa cần sao chép.
 * Đầu ra:
 * - char*: Con trỏ đến vùng nhớ đệm trên Kernel space chứa dữ liệu đã được sao chép từ user space.
 */
char* User2System(int virtAddr,int limit) {
	int i;
	int oneChar = 0;
	
	char* kernelBuf = NULL;
	kernelBuf = (char*) malloc(limit + 1); // chứa thêm kí tự ngắt chuỗi
	if (kernelBuf == NULL)
		return kernelBuf;
	
	memset(kernelBuf,0,limit+1);
	
	for (i = 0 ; i < limit ;i++) {
		kernel->machine->ReadMem(int(virtAddr+i),1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
/*
 * Hàm sao chép chuỗi bytes từ kernel space sang user space.
 * Đầu vào:
 * - int virtAddr: Địa chỉ vùng nhớ trên user space.
 * - int len: Số byte tối đa cần sao chép.
 * - char* buffer: Con trỏ đến vùng nhớ trên kernel space.
 * Đầu ra:
 * - int: Số byte đã được sao chép.
 */
int System2User(int virtAddr,int len,char* buffer) {
	if (len < 0) return -1;
	if (len == 0) return len;
	int i = 0;
	int oneChar = 0 ;
	do {
		oneChar= (int) buffer[i];
		kernel->machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	} while (i < len && oneChar != 0);
	return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
		case SyscallException:
			switch(type) {
				case SC_Halt: {
					DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

					SysHalt();

					ASSERTNOTREACHED();
					break;
				}
				//-----------------------------------------------
				case SC_Add: {
					DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
					
					/* Process SysAdd Systemcall*/
					int result;
					result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
							/* int op2 */(int)kernel->machine->ReadRegister(5));

					DEBUG(dbgSys, "Add returning with " << result << "\n");
					/* Prepare Result */
					kernel->machine->WriteRegister(2, (int)result);
					
					IncreasePC();
					return;
					ASSERTNOTREACHED();
					break;
				}
				//-----------------------------------------------
				case SC_RandomNum: {
					DEBUG(dbgSys, "System call RandomNum invoked!");
					int generatedRandomNum;
					generatedRandomNum = SysRandomNum();
					DEBUG(dbgSys, "RandomNum returning with " << generatedRandomNum << "\n");
					kernel->machine->WriteRegister(2, (int)generatedRandomNum);
					IncreasePC();
					return;
					ASSERTNOTREACHED();
					break;
				}
				//-----------------------------------------------
				case SC_ReadString: {
					DEBUG(dbgSys, "Syscall ReadString invoked!");

					// Lấy tham số từ thanh ghi 4 và 5
					int virtAddr, length;
					virtAddr = kernel->machine->ReadRegister(4);
					length = kernel->machine->ReadRegister(5);
					DEBUG(dbgSys, "SC_ReadString: Kernel space buffer address is " << virtAddr);
					DEBUG(dbgSys, "SC_ReadString: Number of characters to read: " << length);

					// Tạo kernel space buffer
					char* buffer;
					buffer = (char*) malloc(length + 1);
					if (buffer == NULL) {
						DEBUG(dbgSys, "SC_ReadString: ERROR: Failed to allocate memory for kernel space buffer");
						break;
					}

					// Đọc chuỗi từ console vào trong kernel space buffer
					SysReadString(buffer, length);

					DEBUG(dbgSys, "SC_ReadString: ReadString result: " << buffer << "\n");
					DEBUG(dbgSys, "SC_ReadString: ASCII Codes:");
					for (int i = 0; i < length + 1; i++) {
						DEBUG(dbgSys, (int)buffer[i]);
					}

					// Copy chuỗi từ kernel space buffer sang user space buffer
					System2User(virtAddr, length + 1, buffer);

					// Giải phóng bộ nhớ
					free(buffer);

					// Tăng program counter
					IncreasePC();

					return;
					ASSERTNOTREACHED();
					break;
				}
				//-----------------------------------------------
				case SC_PrintString: {
					DEBUG(dbgSys, "Syscall PrintString invoked!");

					// Lấy tham số từ thanh ghi 4
					int virtAddr;
					virtAddr = kernel->machine->ReadRegister(4);
					DEBUG(dbgSys, "SC_PrintString: Kernel space buffer address is " << virtAddr);

					// Chuyển chuỗi từ user space sang kernel space
					char* buffer;
					buffer = NULL;
					const int MAX_STRING = 2048;
					buffer = User2System(virtAddr, MAX_STRING);

					if (buffer == NULL) {
						DEBUG(dbgSys, "SC_PrintString: Failed to copy User2System");
						break;
					}

					DEBUG(dbgSys, "SC_PrintString: kernel space buffer content: " << buffer);

					// Write chuỗi ra console
					SysPrintString(buffer, MAX_STRING);

					// Giải phóng bộ nhớ
					free(buffer);

					// Tăng Program Counter
					IncreasePC();

					return;
					ASSERTNOTREACHED();
					break;
				}
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
