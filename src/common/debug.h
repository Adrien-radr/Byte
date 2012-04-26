#ifndef BYTE_DEBUG_H
#define BYTE_DEBUG_H

#include <stdio.h>
#include <errno.h> 
#include <string.h>
#include <stdbool.h>

//  ##########################################3
//      DEBUG MSG
//  ##########################################3

// DEBUG MACROS slightly modified, originally from Zed Shaw.
#ifdef NDEBUG
#   define debug(M, ...)
#else 
#   define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M , __FILE__, __LINE__, ##__VA_ARGS__) 
#endif

// Function to get time for the log
// Defaults returns 0. Client and Server define theirs (Client_init & Server_init)
float LogClockFunc(); 

#   define WriteLog( M, ... ) do { \
                                if( LogOpened ) { \
                                    fprintf( Log, M, ##__VA_ARGS__ ); \
                                } \
                              } while( 0 )

#   define write_and_log( HEAD, M, ... )  do {\
                                            WriteLog( "<%.2f> [%s] (%s:%d) ", LogClockFunc(), HEAD, __FILE__, __LINE__ ); \
                                            WriteLog( M, ##__VA_ARGS__ );\
                                            printf( "<%.2f> [%s] (%s:%d) ", LogClockFunc(), HEAD, __FILE__, __LINE__ ); \
                                            printf( M, ##__VA_ARGS__ );\
                                          } while(0)

#   define clean_errno() (errno == 0 ? "None" : strerror(errno))
 
#   define log_err(M, ... ) write_and_log( "ERROR", M, ##__VA_ARGS__ );
#   define log_warn(M, ...) write_and_log( "WARN", M, ##__VA_ARGS__ );
#   define log_info(M, ...) write_and_log( "INFO", M, ##__VA_ARGS__ );
                                


#   define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; } 

#   define check_mem(A) check((A), "Out of memory.")
 
#   define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; } 


//  ##########################################3
//      MEMORY MANAGER
//  ##########################################3x
# define MEMORY_MANAGER_SIZE 8192
typedef struct {
    size_t  allocated_bytes;
    void*   alloc_stack[MEMORY_MANAGER_SIZE];
    size_t  alloc_sizes[MEMORY_MANAGER_SIZE];
    int     alloc_lines[MEMORY_MANAGER_SIZE];
    char    alloc_files[MEMORY_MANAGER_SIZE][32];

    int     alloc_cpt;
} MemoryManager;
    
/// Initialize memory manager (call this at the very begining of program)
bool MemoryManager_init();

/// Destroy the Memory manager (call this at the very end of program)
/// This will also print a Report on any memory leak (will free them in the process)
void MemoryManager_destroy();

/// Allocation accounting function (keep account on an allocation, do not allocate anything)
void MemoryManager_allocation( void *ptr, size_t size, const char *file, int line );

/// Deallocation accounting function (keep account on a deallocation, do not free anything)
void MemoryManager_reallocation( void *ptr, void *oldptr, size_t size, const char *file, int line );

/// Reallocation accounting function (keep account on a reallocation, do not reallocate anything)
void MemoryManager_deallocation( void *ptr, const char *file, int line );


//  ##########################################3
//      LOG
//  ##########################################3
// Log file (var defined in common.c
extern FILE *Log;
extern bool LogOpened;

// forward decl
struct s_Clock;

/// Initialize(open) the log with given filename
int InitLog( const char *log_name, struct s_Clock *log_clock );

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
        MemoryManager_allocation( ret, size, file, line );
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
    inline void* byte_realloc_func( void **ptr, size_t size, const char *file, int line ) {
        if( ptr ) {
            if( *ptr ) {    // reallocation
                void *oldptr = *ptr;
                *ptr = realloc( *ptr, size );   
                MemoryManager_reallocation( *ptr, oldptr, size, file, line );
            } else {        // pointer to realloc is null. alloc it
                *ptr = byte_alloc_func( size, file, line );
            }
        }
        return *ptr;
    }

    // Reallocate a pointer and use memory manager in debug mode
#ifndef byte_realloc
#   ifdef _DEBUG
#   define byte_realloc( ptr, size ) byte_realloc_func( (void**)(&ptr), (size), __FILE__, __LINE__ )
#   else
#   define byte_realloc( ptr, size ) realloc( (ptr), (size) )
#   endif
#endif

    // Free a pointer and set it to NULL (tell memory managment)
    inline void byte_dealloc_func( void **ptr, const char *file, int line ) {
        if( ptr && *ptr ) {
            MemoryManager_deallocation( *ptr, file, line );  
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



#endif
