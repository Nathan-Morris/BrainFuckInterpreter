#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/stat.h>

#define BRACKET_INDEX_NPOS  SIZE_MAX

#define BRACKET_ITERATE_MODE_FORWARDS   1
#define BRACKET_ITERATE_MODE_BACKWARDS  0

static size_t bfBracketComplimentIndex(char* instructions, size_t instructionsSize, size_t bracketIndex, char mode) {
    unsigned short bracketCheck = 0;

    if (mode == BRACKET_ITERATE_MODE_BACKWARDS) { // backwards iteration, find '[ 
        for (; bracketIndex != instructionsSize; bracketIndex--) {
            switch (instructions[bracketIndex])
            {
                case '[':
                    bracketCheck--;
                    if (!bracketCheck) {
                        return bracketIndex;
                    }
                    break;

                case ']':
                    bracketCheck++;
                    break;
            }
        }
    } else { // forwards iteration, find ']'
        for (; bracketIndex != instructionsSize; bracketIndex++) {
            switch (instructions[bracketIndex])
            {
                case '[':
                    bracketCheck++;
                    break;

                case ']':
                    bracketCheck--;
                    if (!bracketCheck) {
                        return bracketIndex;
                    }
                    break;
            }
        }
    }

    return BRACKET_INDEX_NPOS;
}

static void bfInterpret(char* instructions, size_t instructionsSize, size_t stackSize) {
    char* stack = (char*)calloc(stackSize, 1), * ptr;
    unsigned int bracketChk;

    if (!stack) {
        return;
    }

    ptr = stack;

    for (size_t instructi = 0, iterInstructi; instructi != instructionsSize; instructi++) {
        switch(instructions[instructi]) {
            case '[':
                if (*ptr == 0) {
                    iterInstructi = bfBracketComplimentIndex(
                        instructions, 
                        instructionsSize,
                        instructi, 
                        BRACKET_ITERATE_MODE_FORWARDS
                    );

                    if (iterInstructi == BRACKET_INDEX_NPOS) {
                        printf("Error At Index: %u | No Matching ']' Bracket Found\n", (unsigned int)instructi);
                        goto out;
                    }

                    instructi = iterInstructi;
                }
                break;

            case ']':       
                if (*ptr) {
                    iterInstructi = bfBracketComplimentIndex(
                        instructions, 
                        instructionsSize,
                        instructi, 
                        BRACKET_ITERATE_MODE_BACKWARDS
                    );

                    if (iterInstructi == BRACKET_INDEX_NPOS) {
                        printf("Error At Index: %u | No Matching '[' Bracket Found\n", (unsigned int)instructi);
                        goto out;
                    }        

                    instructi = iterInstructi;
                }     
                break;

            case '>':
                if (stack + stackSize == ptr) {
                    printf("Error At Index: %u | Stack Pointer Overflow\n", (unsigned int)instructi);
                    goto out;
                }
                ptr++;
                break;

            case '<':
                if (ptr == stack) {
                    printf("Error At Index: %u | Stack Pointer Underflow\n", (unsigned int)instructi);
                    goto out;
                }     
                ptr--;
                break;

            case '+':         
                (*ptr)++;
                break;

            case '-':        
                (*ptr)--;
                break;

            case '.':    
                putc(*ptr, stdout);
                break;

            case ',':
                *ptr = (char)getc(stdin);
                break;

            default:
                continue;
        }
    }

    out:

    free(stack);
}

int main(int argc, char** argv, char** env) {
    size_t stackSize = 128;

    char* data, * end;
    size_t dataLen;

    struct stat fileInfo;
    FILE* fin;

    if (argc <= 1) {
        printf("Usage:\n\tbf.exe \"<instructions>\" <stack size|default=128>\n\tbf.exe /path/to/file <stack size|default=128>\n");
        return 0;
    }

    if (stat(argv[1], &fileInfo) == 0) {
        data = (char*)malloc(fileInfo.st_size);

        if (!data) {
            printf("Unable To Allocate File Content Buffer\n");
            return 0;
        }

        fin = fopen(argv[1], "rb");

        if (fin == NULL || fread(data, 1, fileInfo.st_size, fin) != fileInfo.st_size) {
            free(data);
            printf("Unable To Read File Contents\n");
            return 0;
        }

        dataLen = (size_t)fileInfo.st_size;
    } else {
       data = argv[1];
       dataLen = strlen(data);
    }


    if (argc >= 3) {
        stackSize = (size_t)strtoul(argv[2], &end, 10);

        if (*end != '\0' || !stackSize) {
            printf("Argument '%s' Is Not A Valid Stack Size, Defaulting To '128'\n", argv[2]);
            stackSize = 128;
        }
    }


    bfInterpret(data, dataLen, stackSize);


    printf("\nDone...\n");
}