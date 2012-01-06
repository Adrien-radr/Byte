#ifndef BYTE_DEBUG_H
#define BYTE_DEBUG_H

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdbool.h>

// Log file (var defined in common.c
extern FILE *Log;
extern bool LogOpened;
extern const char LogFile[];

/// Initialize(open) the log file defined in LogFile[]
int InitLog();

/// Close the Log File
inline void CloseLog() {
    fclose( Log );
}

/// Write a message in the log file
#define WriteLog( M, ... )  do {\
                                if( LogOpened ) \
                                    fprintf( Log, M, ##__VA_ARGS__ );\
                            } while(0)

// DEBUG MACROS slightly modified, originally from Zed Shaw.
#ifdef NDEBUG
#   define debug(M, ...)
#else 
#   define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M , __FILE__, __LINE__, ##__VA_ARGS__) 
#endif

#   define write_and_log( HEAD, M, ... )  do {\
                                            char m[256]; \
                                            snprintf( m, 256, M, ##__VA_ARGS__ );\
                                            printf( "[%s] (%s:%d) %s", HEAD, __FILE__, __LINE__, m );\
                                            WriteLog( "[%s] %s", HEAD, m );\
                                          } while(0)

#   define clean_errno() (errno == 0 ? "None" : strerror(errno))
 
#   define log_err(M, ... ) write_and_log( "ERROR", M, ##__VA_ARGS__ );
#   define log_warn(M, ...) write_and_log( "WARN", M, ##__VA_ARGS__ );
#   define log_info(M, ...) write_and_log( "INFO", M, ##__VA_ARGS__ );
                                


#   define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define check_mem(A) check((A), "Out of memory.")
 
#   define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; } 

#endif
