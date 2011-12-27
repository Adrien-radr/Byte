#ifndef BYTE_DEBUG_H
#define BYTE_DEBUG_H

// Debug Header macros by Zed Shaw. Thanks mate!

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdbool.h>

#ifdef NDEBUG
#   define debug(M, ...)
#else 
#   define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 
#endif

#   define clean_errno() (errno == 0 ? "None" : strerror(errno))
 
#   define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__) 
 
#   define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__) 
 
#   define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__) 

#   define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define check_mem(A) check((A), "Out of memory.")
 
#   define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; } 

// Log file (var defined in common.c
extern FILE *Log;
extern bool LogOpened;
extern const char LogFile[];

/// Initialize(open) the log file defined in LogFile[]
int InitLog();

/// Close the Log File
void CloseLog();

/// Write a message in the log file
void WriteLog( const char *msg );

#endif
