#include "syscall.h"

int main() {
    int result;
    result = RandomNum();
    PrintString("Result: ");
    PrintNum(result);
    PrintChar('\n');
    Halt();
}