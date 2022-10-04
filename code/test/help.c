#include "syscall.h"

int main() {
    PrintString("========== Operating System, CNTN20, fit@hcmus ==========\n");
    PrintString("------------------ PROJECT 01: NACHOS -------------------\n");
    PrintString("--------------------- GROUP MEMBERS ---------------------\n");
    PrintString("> 20120250 Tran Bao Anh\n");
    PrintString("> 2012xxxx Nguyen Thien An\n");
    PrintString("> 2012xxxx Huynh Le An\n");
    PrintString("\n=========================================================\n");
    PrintString("====================== HOW TO USE? ======================\n");
    PrintString("\nTo print the ASCII table:\n");
    PrintString("> nachos -x ascii\n");
    PrintString("\nTo sort integer array:\n");
    PrintString("> nachos -x bubblesort\n");
    Halt();
}