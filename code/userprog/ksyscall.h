/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysRandomNum() {
  srand(time(NULL));
  return rand();
}

void SysReadString(char* buffer, int length) {
  for (int i = 0; i < length; i++) {
    char ch;
    ch = (char) kernel->synchConsoleIn->GetChar();
    buffer[i] = ch;
  }
  buffer[length] = '\0';
}

void SysPrintString(char* buffer, int limit) {
  if (buffer != NULL) {
    int i = 0;
    while (i < limit && (char)buffer[i] != '\0') {
      kernel->synchConsoleOut->PutChar((char)buffer[i]);
      i++;
    }
  }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
