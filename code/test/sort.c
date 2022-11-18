#include "syscall.h"

int GoodOrder(int curr, int next, int asc) {
    if (asc == 1)
        return (curr <= next) ? 1 : 0;
    else
        return (curr >= next) ? 1 : 0;
}

void BubbleSort(int* a, int n, int asc) {
    int left, right;
    left = 0;
    right = n - 1;
    while (left <= right) {
        int isSwapped, lastSwapPos, j;
        isSwapped = 0;
        lastSwapPos = left + 1;
        for (j = left; j < right; ++j) {
            if (!GoodOrder(a[j], a[j+1], asc)) {
                int temp;
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
                isSwapped = 1;
                lastSwapPos = j + 1;
            }
        }
        if (isSwapped == 0)
            break;
        right = lastSwapPos;
    }
}

int main() {
    
    int n;
    int a[100];
    int isAscending, i;
    
    // Get size of array
    do {
        PrintString("Enter Size of array: ");
        n = ReadNum();

        if (n < 1 || n > 100)
            PrintString("Invalid size! Please enter size as an integer in range [1, 100]\n");

    } while (n < 1 || n > 100);

    // Get array elements' values
    PrintString("Enter elements' values:\n");
    
    for (i = 0; i < n; i++) {
        PrintString("Enter value of array[");
        PrintNum(i);
        PrintString("]: ");
        a[i] = ReadNum();
    }
    
    // Get user choosen sort order
    PrintString("Ascending or Descending (Enter 1 for ascending, 2 for descending): ");
    do {
        isAscending = ReadNum();
        if (!(isAscending == 1 || isAscending == 2))
            PrintString("Invalid choosen! Please enter 1 for ascending, 2 for descending)\n");
    } while (isAscending != 1 && isAscending != 2);

    PrintString("\nArray: ");
    for (i = 0; i < n; i++) {
        PrintNum(a[i]);
        PrintChar(' ');
    }

    BubbleSort(a, n, isAscending);

    PrintString("\nResult: ");
    for (i = 0; i < n; i++) {
        PrintNum(a[i]); PrintChar(' ');
    }
    PrintChar('\n');

    Halt();
}