#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "debug_macros.h"
#include "logfind.h"


static uint32_t globalMatchCount = 0;

// Sending a stirng literal to the stdout
void sendMessage(char mode, const char *fmt, ...)
{
    int i = 0;
    int rc = 0;
    unsigned int integer = 0;
    int character = ' ';
    char int_str[12];
    char *str;

    va_list vargs;
    va_start(vargs, fmt);

    // setting up modes for better interface
    switch(mode) {
        case 'i':
            printf("[INFO]: ");
            break;
        case 'w':
            printf("[WARNING]: ");
            break;
        case 'e':
            printf("[ERROR]: ");
            break;
        default:
            sentinel("Wrong mode! use i for info, w for warning, e for error and try again.");
    }

    // Iterating over the format string and printing each character or related string format
    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            // Sending string formats for each case to the stdout
            switch(fmt[++i]) {
                case '\0':
                    sentinel("Input contains single %%, if you intended to use one,\
                            use double %%%%.");
                case '%':
                    rc = fputc('%', stdout);
                    check(rc != EOF, "Failed to write character into stdout.");
                    break;
                case 'd':
                    integer = va_arg(vargs, unsigned int);
                    rc = sprintf(int_str, "%d", integer);
                    check(rc != 0, "Failed in converting integer to string.");
                    fputs(int_str, stdout);
                    break;
                case 'c':
                    character = va_arg(vargs, int);
                    rc = fputc(character, stdout);
                    check(rc != EOF, "Failed to write character into stdout.");
                    break;
                case 's':
                    str = va_arg(vargs, char *);
                    fputs(str, stdout);
                    break;
                default:
                    sentinel("Format error!");
            }
        } else {
            fputc(fmt[i], stdout);
        }

        // Checking that there is no error coming from the shell (stdout)
        check(!feof(stdout) && !ferror(stdout), "Input error!");

    }
    fputc('\n', stdout);

    va_end(vargs);

error:
    va_end(vargs);
}

// Sending result to stdout for each line of the corresponding log file
void outputResult(char *filePath, uint16_t matchingLine, uint8_t matchCount, char *lineContent)
{
    sendMessage('i', "[\"%s\"]: At line %d, found %d matches: \n \
            \"%s\"", filePath, matchingLine, matchCount, lineContent);

    // Incrementing global match count variable for a total count
    globalMatchCount += matchCount;
}

int processInput(int argc, char **argv)
{
    int i, j, count = 0;
    char mode;
    char *newArgv[argc];

    // Handling cases by their argument count ( Can be better, possible place for a refactor)
    switch(argc) {
        case 1:
            sendMessage('e', "There's no arguments given, \
type the strings you want to search for and put spacebar between them.\
Type -o for searching for arguments with OR logic. The default one is AND logic.");
            exit(EXIT_SUCCESS);
        default:
            // Handling the optional arguments given by the user

            // OR LOGIC
            if (!strcmp(argv[1], "-o")) {
                for (i = 2, j = 0; i < argc; i++, j++) {
                    newArgv[j] = argv[i];
                }
                mode = 'o';
                count = argc - 2;

                // HELP COMMAND
            } else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
                sendMessage('i', "Write the list of words to search for with spacebar between them. \n\
                        Default searching is with AND logic.\n  Flag options:\n -o ==> OR logic\n\
-h or --help ==> getting some info about the usage.");
                exit(EXIT_SUCCESS);

            // AND LOGIC 
            } else {
                for (i = 1, j = 0; i < argc; i++, j++) {
                    newArgv[j] = argv[i];
                }
                mode = 'a';
                count = argc - 1;
            }
    }

    // Initiating the process of searching in log files
    processLogFiles(newArgv, count, mode); 

    return 0;
}

int main(int argc, char **argv)
{
    double elapsed_time;
    clock_t start_time = clock();
    int ret = 0;

    // Passing argc and argv as arguments to process
    ret = processInput(argc, argv);
    check(ret == 0, "An error occured while processing input from stdin.");

    // Sending different messages depending on match count
    if (globalMatchCount == 0)
        sendMessage('w', "There were no matching words found in log files.");
    else 
        sendMessage('i', "Found a total of %d matching words.", globalMatchCount);

    // Benchmark facilities
    elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("[BENCHMARK]: %g s\n", elapsed_time);

    exit(EXIT_SUCCESS);

error:
    elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("[BENCHMARK]: %g s\n", elapsed_time);
    exit(EXIT_FAILURE);
}
