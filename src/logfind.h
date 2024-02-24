/****       CREATED BY SHINKONOMI @ github.com      ****\
 ****                PROJECT LOGFIND                ****
 ****                 MIT LICENSE                   ****
 ****                  JAN 2024                     ****
 ****/
#ifndef __LOGFIND_H__
#define __LOGFIND_H__

#include <stdio.h>
#include <stdint.h>

#define LOGFIND_PATH "/home/user"
#define FILESTRING_SIZE 262144
#define MAX_LOG_FILES 128

/* Each interface in each subsystem */

// File Handler
int processLogFiles(char **searchList, int searchListCount, char mode);

// IO Interface
void sendMessage(char mode, const char *fmt, ...);
void outputResult(char *filePath, uint16_t matchingLine, uint8_t matchCount, char *lineContent); 

// String Processor
uint8_t logFileStringLoader(char **logFindFileContentList, char *logFindFileContent);
int searchForStrings(char *logContent, char **searchList, int searchListCount, char *logName, char mode);

#endif // LOGFIND_H
