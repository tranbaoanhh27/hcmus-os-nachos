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

#define __STDC_LIMIT_MACROS
#include "kernel.h"
#include "synchconsole.h"
#include "syscall.h"
#include <stdint.h>

#define _maxNumLen 11
char _numberBuffer[_maxNumLen + 2];

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysRandomNum()
{
  srand(time(NULL));
  return rand() + 1;
}

void SysReadString(char *buffer, int length)
{
  for (int i = 0; i < length; i++)
  {
    char ch;
    ch = (char)kernel->synchConsoleIn->GetChar();
    buffer[i] = ch;
  }
  buffer[length] = '\0';
}

void SysPrintString(char *buffer, int limit)
{
  if (buffer != NULL)
  {
    int i = 0;
    while (i < limit && (char)buffer[i] != '\0')
    {
      kernel->synchConsoleOut->PutChar((char)buffer[i]);
      i++;
    }
  }
}

bool checkNumString(int integer, const char *s)
{
  if (integer == 0)
    return strcmp(s, "0") == 0;

  int len = strlen(s);

  if (integer < 0 && s[0] != '-')
    return false;

  if (integer < 0)
    s++, --len, integer = -integer;

  while (integer > 0)
  {
    int digit = integer % 10;

    if (s[len - 1] - '0' != digit)
      return false;

    --len;
    integer /= 10;
  }

  return len == 0;
}

int SysReadNum()
{
  memset(_numberBuffer, 0, sizeof(_numberBuffer));
  int n = 0;
  char c;

  // read the input from the keyboard
  while (1)
  {
    c = kernel->synchConsoleIn->GetChar();
    // if user get enter -> stop
    if (c != '\n')
      _numberBuffer[n++] = c;
    else
      break;
  }

  // because when we convert numb -> -num to compare string in function checkNumString
  // if it has wrong value
  if (strcmp(_numberBuffer, "-2147483648") == 0)
    return INT32_MIN;

  // get the len of input 
  int len = strlen(_numberBuffer);

  // if over the range
  if (len >= _maxNumLen)
  {
    DEBUG(dbgSys, "Have an error");
    return 0;
  }

  int num = 0;
  bool nega = (_numberBuffer[0] == '-');
  int index = nega;

  // get the int value of characters
  while (index < len)
  {
    char temp = _numberBuffer[index++];
    if (temp < '0' || temp > '9')
    {
      DEBUG(dbgSys, "Have an error");
      return 0;
    }

    num = num * 10 + int(temp - '0');
  }

  if (nega)
    num = -num;

  // check if stackoverflow occurs
  // or wrong input like "-012"
  if (checkNumString(num, _numberBuffer))
    return num;
  else
    DEBUG(dbgSys, "Have an error");

  return 0;
}

void SysPrintNum(int num)
{
  // check if num == 0
  // because we need num == 0 for loop condition
  if (num == 0)
    return kernel->synchConsoleOut->PutChar('0');

  // the -num value is over the INT32 so cannot num = -num
  // and use loop to print
  if (num == INT32_MIN)
  {
    kernel->synchConsoleOut->PutChar('-');
    for (int i = 0; i < 10; ++i)
      kernel->synchConsoleOut->PutChar("2147483648"[i]);
    return;
  }

  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
    num = -num;
  }
  int n = 0;
  while (num)
  {
    _numberBuffer[n++] = num % 10;
    num /= 10;
  }
  for (int i = n - 1; i >= 0; --i)
    kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

char SysReadChar()
{
  return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char character)
{
  kernel->synchConsoleOut->PutChar(character);
}

int SysCreate(char* name) {
  return kernel->fileSystem->Create(name);
}

OpenFileId SysOpen(char* fileName, int type = 0) {
    if (type != 0 && type != 1) return -1;

    int id = kernel->fileSystem->OpenMode(fileName, type);
    if (id == -1) return -1;
    return id;
}

int SysClose(OpenFileId id) {
  return kernel->fileSystem->Close(id);
}

int SysWrite(char* buffer, int size, OpenFileId id) {
  if (id == 0) {
    return -1;
  }
  else if (id == 1) {
    SysPrintString(buffer, size);
    return 1;
  }
  return kernel->fileSystem->Write(buffer, size, id);
}

int SysRead(char* buffer, int size, OpenFileId id) {
  if (id == 0) {
    SysReadString(buffer, size);
    return 1;
  }
  else if (id == 1) {
    return -1;
  }
  return kernel->fileSystem->Read(buffer, size, id);
}

int SysSeek(int pos, OpenFileId id) {
  return kernel->fileSystem->Seek(pos, id);
}

int SysRemove(char* fileName) {
  return kernel->fileSystem->Remove(fileName);
}

// void SysCopyUserKernelUser() {

// }

#endif /* ! __USERPROG_KSYSCALL_H__ */
