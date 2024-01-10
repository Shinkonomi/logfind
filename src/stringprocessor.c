#include <stdint.h>

#include "debug_macros.h"
#include "logfind.h"

#define LINESTRING_SIZE 256
#define MATCHINGLINES_SIZE 16384

// Reading each line of .logfind file and inserting it
// to the list given by arguments
uint8_t logFileStringLoader(char **fileContentList, char *fileContent)
{
    int i, j, logCount = 0;
    check_mem(fileContentList && fileContent);

    // Iterating over content until NULL termination
    for (i = 0, j = 0, logCount = 0; fileContent[i] != '\0'; i++, j++) {
        if (j >= 63)
            sentinel("Some log file name in .logfind is bigger than 63 characters");
        if (fileContent[i] == '\n') {
            // After reading each line it null terminates it
            // and increments log count
            fileContentList[logCount][j] = '\0';
            ++logCount;
            j = -1;
            continue;
        }
        fileContentList[logCount][j] = fileContent[i]; 
    }

    return logCount;

error:
    return -1;
}

// Searchs for the second given string in the first given string
uint8_t isStringHere(char *main, char *searchFor)
{
    int i, j, size = 0;
    uint8_t count = 0;
    
    // Getting the size of search string
    for (size = 0; searchFor[size] != '\0'; size++) {
    }

    // Uses quite the same logic used in log file string loader
    for (i = 0, j = 0, count = 0; main[i] != '\0'; i++) {
        if (main[i] == searchFor[j]) {
            ++j;
            if (j == size) {
                ++count;
                j = 0;
            }
            continue;
        }
        j = 0;
    }

    // Returns the number of times it encountered the second string
    return count;
}

int searchForStrings(char *logContent, char **searchList, int searchListCount, char *logName, char mode)
{
    int i, j, outputFlag = 0;
    uint8_t matchCount = 0;
    uint16_t currentLine = 0;
    char currentLineString[LINESTRING_SIZE];
    check_mem(logContent && logName && searchList);

    // Handles each logic by different approach
    switch(mode) {
        // AND LOGIC
        case 'a':
            // Iterates over the log content and processes each line
            for (i = 0, j = 0, matchCount = 0, currentLine = 1; logContent[i] != '\0'; i++, j++) {
                if (logContent[i] == '\n') {
                    currentLineString[j] = '\0';
                    int k = 0;
                    for (k = 0; k < searchListCount; k++) {
                        int count = isStringHere(currentLineString, searchList[k]);
                        if (count) {
                            // It only increments flag and match count if that string is there
                            ++outputFlag;
                            matchCount += count;
                        }
                    }
                    // If every string in the search list is encountered then
                    // it outputs the result
                    if (outputFlag == searchListCount)
                        outputResult(logName, currentLine, matchCount, currentLineString);
                    ++currentLine;
                    matchCount = 0;
                    outputFlag = 0;
                    j = -1;
                    continue;
                }
                currentLineString[j] = logContent[i];
            }
            break;

        // OR LOGIC
        case 'o':
            // Iterates over the log content and processes each line
            for (i = 0, j = 0, matchCount = 0, currentLine = 1; logContent[i] != '\0'; i++, j++) {
                if (logContent[i] == '\n') {
                    currentLineString[j] = '\0';
                    int k = 0;
                    for (k = 0; k < searchListCount; k++) {
                        // If any match were found, it considers it
                        matchCount += isStringHere(currentLineString, searchList[k]);
                    }
                    // If match count was zero it will not output any results
                    if (matchCount)
                        outputResult(logName, currentLine, matchCount, currentLineString);
                    ++currentLine;
                    matchCount = 0;
                    j = -1;
                    continue;
                }
                currentLineString[j] = logContent[i];   
            }
            break;

        default:
            sentinel("Mode input error! Use 'a' for AND logic and 'o' for OR logic.");
    }

    return 0;

error:
    return -1;
}
