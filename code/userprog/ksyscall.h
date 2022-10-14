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
#include <stdint.h>

#define LF ((char)10)
#define CR ((char)13)
#define TAB ((char)9)
#define SPACE ((char)' ')

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

bool isStop(char c) { return c == LF || c == CR || c == TAB || c == SPACE; }

bool checkINT32(int integer, const char *s)
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

  while (n < _maxNumLen)
  {
    c = kernel->synchConsoleIn->GetChar();
    if (c != '\n')
      _numberBuffer[n++] = c;
    else
      break;
  }

  // because when we convert numb -> -num to compare string in function checkINT32
  // it has stackoverflow error
  if (strcmp(_numberBuffer, "-2147483648") == 0)
    return INT32_MIN;

  int len = strlen(_numberBuffer);

  if (len > _maxNumLen)
  {
    DEBUG(dbgSys, "Have an error: over the range");
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
      DEBUG(dbgSys, "Have an error: have a character diff number");
      return 0;
    }

    num = num * 10 + int(temp - '0');
  }

  if (nega)
    num = -num;

  // check if stackoverflow occurs
  if (checkINT32(num, _numberBuffer))
    return num;
  else
    DEBUG(dbgSys, "Have an error: stackoverflow");

  return 0;
}

void SysPrintNum(int num)
{
  for (int i = 0; i < 9; i++)
  {
    kernel->synchConsoleOut->PutChar("number : "[i]);
  }

  if (num == 0)
    return kernel->synchConsoleOut->PutChar('0');

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

  kernel->synchConsoleOut->PutChar('\n');
}

char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

void SysPrintChar(char character)
{

  for (int i = 0; i < 13; i++)
  {
    kernel->synchConsoleOut->PutChar("character : "[i]);
  }

  kernel->synchConsoleOut->PutChar(character);
  kernel->synchConsoleOut->PutChar('\n');
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
