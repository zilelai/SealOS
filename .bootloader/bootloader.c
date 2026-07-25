#include <stdio.h>
#include <stdlib.h>

int bootloader;

int main() {
    system("clear");
    while (1) {
        printf("SealKernel Bootloader\n");
        printf("-(1)SealKernel Latest\n");
        printf("-(2)SealKernel Stable\n");
        printf("-(3)Exit\n");
        printf("Please Select One: ");

        if (scanf("%d", &bootloader) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf(" \n");
            printf("Bootloader Error: Index %d Does Not Exist Here. Select Again\n\n", bootloader);
            continue;
        }

        if (bootloader == 1) {
            system("clear");
            system("./latest");
        }
        else if (bootloader == 2) {
            system("clear");
            system("./backup");
        }
        else if (bootloader == 3) {
            system("clear");
            exit(0);
        }
        else {
            printf(" \n");
            printf("Bootloader Error: Index %d Does Not Exist Here. Select Again\n\n", bootloader);
            continue;
        }
    }
    return 0;
}

