#include "common.h"
#include "clock.h"

//  ################################
//      LOG
//  ################################
FILE *Log;
bool LogOpened = false;
struct s_Clock *LogClock = NULL;

int InitLog( const char *log_name, struct s_Clock *log_clock ) {
    Log = fopen( log_name, "w" );

    if( !Log ) {
        log_err( "Error while opening log \"%s\"", log_name );
        return 0;
    }

    LogOpened = true;
    LogClock = log_clock;

    str16 time;
    str64 date;
    GetTime( date, 64, DateFmt );
    GetTime( time, 16, TimeFmt );
    strncat( date, " - ", 3 );  
    strncat( date, time, 16 );

    WriteLog( "Byte-Project Log v%d.%d.%d\n", BYTE_MAJOR, BYTE_MINOR, BYTE_PATCH );
    WriteLog( "%s\n", date );
    WriteLog( "==========================\n" );

    return 1;
}

extern inline void CloseLog();

inline float LogClockFunc() {
    return LogClock ? Clock_getElapsedTime( LogClock ) : 0.f;
}

//  ################################
//      MEMORY MANAGER
//  ################################
MemoryManager *mem_manager = NULL;

bool MemoryManager_init() {
    if( !mem_manager ) {
        mem_manager = malloc( sizeof( MemoryManager ) );

        if( !mem_manager ) {
            printf( "[ERROR] Memory Manager could not be allocated!\n" );
            return false;
        }

        mem_manager->alloc_cpt = 0;
        mem_manager->allocated_bytes = 0;
        memset( mem_manager->alloc_stack, 0, MEMORY_MANAGER_SIZE * sizeof( void* ) );
        memset( mem_manager->alloc_sizes, 0, MEMORY_MANAGER_SIZE * sizeof( size_t ) );
        memset( mem_manager->alloc_files, 0, MEMORY_MANAGER_SIZE * 32 * sizeof( char ) );
        memset( mem_manager->alloc_lines, 0, MEMORY_MANAGER_SIZE * sizeof( int ) );

        return true;
    }
    return false;
}

void MemoryManager_destroy() {
    if( mem_manager ) {
        // rapport :
        printf( "Memory Manager Rapport :\n"
                "============================\n"
                "Allocated bytes at the end of execution : %zu.\n", mem_manager->allocated_bytes );
        if( mem_manager->allocated_bytes ) {
            printf( "Non-free'd pointers :\n" );
            u32 cpt = mem_manager->alloc_cpt;
            for( u32 i = 0; i < cpt; ++i ) {
                if( mem_manager->alloc_sizes[i] ) {
                    printf( "\tAddress : %p  (%s:%d)\n"
                            "\tSize : %zu\n", mem_manager->alloc_stack[i], 
                                              mem_manager->alloc_files[i],
                                              mem_manager->alloc_lines[i],
                                              mem_manager->alloc_sizes[i] );
                    free( mem_manager->alloc_stack[i] );
                } 
            }
        }
        free( mem_manager );
        mem_manager = NULL; 
    }
}

void MemoryManager_allocation( void* ptr, size_t size, const char *file, int line ) {
    if( mem_manager ){
        u32 cpt = mem_manager->alloc_cpt++;
        if( cpt < MEMORY_MANAGER_SIZE ) {
            mem_manager->allocated_bytes += size;
            mem_manager->alloc_stack[cpt] = ptr;
            mem_manager->alloc_sizes[cpt] = size;
            mem_manager->alloc_lines[cpt] = line;
            //mem_manager->alloc_files[cpt] = file;
            strncpy( mem_manager->alloc_files[cpt], file, 32 );
        } else
            printf( "[ERROR] MemoryManager Allocation stack is not big enough!\n" );
    }
}

void MemoryManager_reallocation( void *ptr, void *oldptr, size_t size, const char *file, int line ) {
    if( mem_manager ) {
        bool found = false;
        for( int i = 0; (i < mem_manager->alloc_cpt) && !found; ++i )
            if( oldptr == mem_manager->alloc_stack[i] && 0 != mem_manager->alloc_sizes[i] ) {
                mem_manager->allocated_bytes += size - mem_manager->alloc_sizes[i];
                mem_manager->alloc_stack[i] = ptr;
                mem_manager->alloc_sizes[i] = size;
                mem_manager->alloc_lines[i] = line;
                //mem_manager->alloc_files[i] = file;
                strncpy( mem_manager->alloc_files[i], file, 32 );
                found = true;
            }
        if( !found )
            log_err( "MemoryManager error (%s:%d): Tried to reallocate pointer \"%p\", but it has never been allocated in the memory manager before!\n", file, line, ptr );
    }
}

void MemoryManager_deallocation( void* ptr, const char *file, int line ) {
    if( mem_manager ) {
        bool found = false;
        for( int i = 0; (i < mem_manager->alloc_cpt) && !found; ++i )
            if( ptr == mem_manager->alloc_stack[i] && 0 != mem_manager->alloc_sizes[i] ) {
                mem_manager->allocated_bytes -= mem_manager->alloc_sizes[i];
                mem_manager->alloc_sizes[i] = 0;
                found = true;
            }
        if( !found )
            log_err( "MemoryManager error (%s:%d): Tried to deallocate pointer \"%p\", but it has never been allocated in the memory manager before!\n", file, line, ptr );
    }
}



//  ################################
//      MEMORY ALLOCATORS
//  ################################
extern inline void* byte_alloc_func( size_t size, const char* file, int line );
extern inline void* byte_realloc_func( void **ptr, size_t size, const char *file, int line );
extern inline void byte_dealloc_func( void **ptr, const char *file, int line );




