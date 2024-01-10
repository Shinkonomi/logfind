#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "debug_macros.h"
#include "logfind.h"

typedef struct LogFiles {
    uint8_t logCount;
    FILE **logFilesList;
    char **logFilesNames;
} LogFiles;

// Function to load the corresponding log files into logfiles structure
int loadLogFiles(LogFiles *logFiles)
{
    int i, j, ret = 0;
    FILE *logFindFile = NULL;
    char **logFilesList = NULL;
    char *logFindFileContent = NULL;
    check(logFiles != NULL, "No log files structure were found.");

    // Opening the .logfind main file
    logFindFile = fopen(LOGFIND_PATH, "r+");
    check(logFindFile != NULL, "Can't open the .logfind file.");

    // Allocating memory to the logfileslist string list
    // Max logFilenames with each 64 bytes allocated to
    logFilesList = calloc(MAX_LOG_FILES, sizeof(*logFilesList));    
    check_mem(logFilesList);
    // Allocating heap to each string
    for (i = 0; i < MAX_LOG_FILES; i++) {
        logFilesList[i] = calloc(64, sizeof(**logFilesList));  
        check_mem(logFilesList[i]);
    }

    // Allocating memory to the content of .logfind
    logFindFileContent = calloc(MAX_LOG_FILES, 64);
    check_mem(logFindFileContent);
    // Reading .logfind to the allocated content string
    ret = fread(logFindFileContent, sizeof(*logFindFileContent), MAX_LOG_FILES * 64, logFindFile);
    check(ret != 0, "An error occured while reading the contents of file.");
    // Null terminating the last character
    logFindFileContent[MAX_LOG_FILES * 64 - 1] = '\0';

    // This function returns the number of log names saved 
    // into the logFilesList
    ret = logFileStringLoader(logFilesList, logFindFileContent);
    check (ret > 0, "An error occured while processing the contents of .logfind.");
    logFiles->logCount = ret;

    // Opening each log file and setting up the logfiles structure
    for (i = 0, j = 0; j < logFiles->logCount; i++, j++) {
        FILE *logFile = fopen(logFilesList[i], "r");
        if (logFile == NULL) {
            sendMessage('e', "Can't open the file at: [\"%s\"]. Going to ignore it.", logFilesList[i]);
            --logFiles->logCount;
            --j;
            continue;
        }
        (logFiles->logFilesList)[j] = logFile;
        (logFiles->logFilesNames)[j] = logFilesList[i];
    }

    // Cleaning up
    fclose(logFindFile);
    for (i = 0; i < MAX_LOG_FILES; i++) {
        free(logFilesList[i]);
        logFilesList[i] = NULL;
    }
    free(logFilesList);
    logFilesList = NULL;
    free(logFindFileContent);
    logFindFileContent = NULL;

    return 0;

error:
    if (logFindFile)
        fclose(logFindFile);
    if (logFilesList) {
        for (i = 0; i < MAX_LOG_FILES; i++) {
            free(logFilesList[i]);
            logFilesList[i] = NULL;
        }
        free(logFilesList);
        logFilesList = NULL;
    }
    if (logFindFileContent) {
        free(logFindFileContent);
        logFindFileContent = NULL;
    }
    return -1;
}

void clearLogFiles(LogFiles *logFiles)
{
    int i = 0;

    for (i = 0; i < logFiles->logCount; i++) {
        if ((logFiles->logFilesList)[i]) {
            fclose((logFiles->logFilesList)[i]);
            (logFiles->logFilesList)[i] = NULL;
        }
    }
}

void removeLogFiles(LogFiles *logFiles)
{
    int i = 0;

    // Removing and freeing each allocated heap memory and setting it to NULL
    if ((logFiles->logFilesNames)) {
        for (i = 0; i < MAX_LOG_FILES; i++) {
            free((logFiles->logFilesNames)[i]);
            (logFiles->logFilesNames)[i] = NULL;
        }
        free((logFiles->logFilesNames));
        (logFiles->logFilesNames) = NULL;
    }
    if (logFiles->logFilesList) {
        clearLogFiles(logFiles);
        free(logFiles->logFilesList);
        logFiles->logFilesList = NULL;
    }
    if (logFiles) {
        free(logFiles);
        logFiles = NULL;
    }
}

LogFiles *createLogFiles(void)
{
    int i = 0;
    LogFiles *logFiles = NULL;

    // Allocating memory to each structure and string
    logFiles = (LogFiles *)malloc(sizeof(LogFiles));
    check_mem(logFiles);
    logFiles->logFilesList = calloc(sizeof(FILE *), MAX_LOG_FILES);
    check_mem(logFiles->logFilesList);
    (logFiles->logFilesNames) = calloc(MAX_LOG_FILES, sizeof(*(logFiles->logFilesNames)));
    check_mem((logFiles->logFilesNames));
    for (i = 0; i < MAX_LOG_FILES; i++) {
        (logFiles->logFilesNames)[i] = calloc(64, sizeof(**(logFiles->logFilesNames)));
        check_mem((logFiles->logFilesNames)[i]);
    }

    return logFiles;

error:
    removeLogFiles(logFiles);
    return NULL;
}

int processLogFiles(char **searchList, int searchListCount, char mode)
{
    int i, ret = 0;
    LogFiles *logFiles = NULL;
    char *logContent = NULL;
    check_mem(searchList);
    
    // Initializing the logfile structure
    logFiles = createLogFiles();

    // Loading up the logfile structure according to .logfind file
    ret = loadLogFiles(logFiles);
    check(ret == 0, "An error occured while loading log files.");

    logContent = calloc(FILESTRING_SIZE, sizeof(*logContent));
    check_mem(logContent);

    // Iterating over each log pathname in logfiles structure and
    // reading the contents of the file
    for (i = 0; i < logFiles->logCount; i++) {
        ret = fread(logContent, sizeof(*logContent), FILESTRING_SIZE, (logFiles->logFilesList)[i]);
        // NULL terminating the end
        logContent[FILESTRING_SIZE - 1] = '\0';
        if (ret == 0) {
            sendMessage('e', "Can't read the content of file at: [%s]. Going to ignore it.",
                    (logFiles->logFilesNames)[i]);
            continue;
        }
        // After reading the file, the process of searching for search
        // list is then initialized
        ret = searchForStrings(logContent, searchList, searchListCount, (logFiles->logFilesNames)[i], mode);
        check(ret == 0, "An error occured while trying to search for given strings in log file at:\
                [%s].", (logFiles->logFilesNames)[i]);
    }

    // Cleaning up
    clearLogFiles(logFiles);
    removeLogFiles(logFiles);
    free(logContent);
    logContent = NULL;

    return 0;

error:
    if (logContent) {
        free(logContent);
        logContent = NULL;
    }
    if (logFiles) {
        clearLogFiles(logFiles);
        removeLogFiles(logFiles);
    }
    return -1;
}
