#ifndef BYTE_DEBUG_H
#define BYTE_DEBUG_H

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdbool.h>

//  ##########################################3
//      MEMORY MANAGER
//  ##########################################3
typedef struct {
    size_t  allocated_bytes;
    void*   alloc_stack[512];
    size_t  alloc_sizes[512];
    int     alloc_lines[512];
    char    alloc_files[512];

    int     alloc_cpt;
} MemoryManager;
    
/// Initialize memory manager (call this at the very begining of program)
bool MemoryManager_init();

/// Destroy the Memory manager (call this at the very end of program)
/// This will also print a Report on any memory leak (will free them in the process)
void MemoryManager_destroy();

/// Allocation accounting function (keep account on an allocation, do not allocate anything)
void MemoryManager_allocation( void *ptr, size_t size, char file, int line );

/// Deallocation accounting function (keep account on a deallocation, do not free anything)
void MemoryManager_reallocation( void *ptr, size_t size, char file, int line );

/// Reallocation accounting function (keep account on a reallocation, do not reallocate anything)
void MemoryManager_deallocation( void *ptr, char file, int line );


//  ##########################################3
//      LOG
//  ##########################################3
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



//  ##########################################3
//      MEMORY ALLOCATORS
//  ##########################################3
    // Allocate a pointer 
    inline void* byte_alloc_func( size_t size, const char* file, int line ) {
        void* ret = calloc( 1, size );
        MemoryManager_allocation( ret, size, file[4], line );
        return ret;
    }

    // Allocate a pointer (with calloc) and use memory manager in debug mode
#ifndef byte_alloc
#   ifdef _DEBUG
#   define byte_alloc( size ) byte_alloc_func( (size), __FILE__, __LINE__ )
#   else
#   define byte_alloc( size ) calloc( 1, (size ) )
#   endif
#endif

    // Reallocate a pointer
    inline void* byte_realloc_func( void *ptr, size_t size, const char *file, int line ) {
        if( ptr ) {
            ptr = realloc( ptr, size );   
            MemoryManager_reallocation( ptr, size, file[4], line );
        }
        return ptr;
    }

    // Reallocate a pointer and use memory manager in debug mode
#ifndef byte_realloc
#   ifdef _DEBUG
#   define byte_realloc( ptr, size ) byte_realloc_func( (ptr), (size), __FILE__, __LINE__ )
#   else
#   define byte_realloc( ptr, size ) realloc( (ptr), (size) )
#   endif
#endif

    // Free a pointer and set it to NULL (tell memory managment)
    inline void byte_dealloc_func( void **ptr, const char *file, int line ) {
        if( ptr && *ptr ) {
            MemoryManager_deallocation( *ptr, file[4], line );  
            free(*ptr);                            
            *ptr = NULL;                        
        }
    }
#ifndef DEL_PTR
#   ifdef _DEBUG
#   define DEL_PTR(p) byte_dealloc_func( (void**)(&p), __FILE__, __LINE__ )
#   else
#   define DEL_PTR(p)           \
        do {                    \
            if( p ) {           \
                free(p);        \
                (p) = NULL;     \
            }                   \
        } while(0)
#   endif
#endif



//  ##########################################3
//      DEBUG MSG
//  ##########################################3

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
                                            char m[512]; \
                                            snprintf( m, 512, M, ##__VA_ARGS__ );\
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
