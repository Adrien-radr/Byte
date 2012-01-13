#include <stdarg.h>

#include "common.h"


// Memory manager
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
        return true;
    }
    return false;
}

void MemoryManager_destroy() {
    if( mem_manager ) {
        // rapport :
        printf( "Memory Manager Rapport :\n"
                "============================\n"
                "Allocated bytes at the end of execution : %ld.\n", mem_manager->allocated_bytes );
        if( mem_manager->allocated_bytes ) {
            printf( "Non-free'd pointers :\n" );
            u32 cpt = mem_manager->alloc_cpt;
            for( u32 i = 0; i < cpt; ++i ) {
                if( mem_manager->alloc_sizes[i] ) {
                    printf( "\tAddress : 0x%ld  (%c:%d)\n"
                            "\tSize : %ld\n", (size_t)mem_manager->alloc_stack[i], 
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

void MemoryManager_allocation( void* ptr, size_t size, char file, int line ) {
    if( mem_manager ){
        u32 cpt = mem_manager->alloc_cpt++;
        if( cpt < 512 ) {
            mem_manager->allocated_bytes += size;
            mem_manager->alloc_stack[cpt] = ptr;
            mem_manager->alloc_sizes[cpt] = size;
            mem_manager->alloc_lines[cpt] = line;
            mem_manager->alloc_files[cpt] = file;
        } else
            printf( "[ERROR] MemoryManager Allocation stack is not big enough!\n" );
    }
}

void MemoryManager_deallocation( void* ptr ) {
    if( mem_manager ) {
        for( int i = 0; i < mem_manager->alloc_cpt; ++i )
            if( ptr == mem_manager->alloc_stack[i] ) {
                mem_manager->allocated_bytes -= mem_manager->alloc_sizes[i];
                mem_manager->alloc_sizes[i] = 0;
                break;
            }
    }
}

// Definitions of debug.h
FILE *Log;
bool LogOpened = false;
const char LogFile[] = "Byte.log";

int InitLog() {
    Log = fopen( LogFile, "w" );

    if( !Log ) {
        log_err( "Error while opening log \"%s\"", LogFile );
        return 0;
    }

    LogOpened = true;

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



// Definitions of common.h
const char DateFmt[] = "%a %d %b %Y";
const char TimeFmt[] = "%H:%M:%S";
 
extern inline void* byte_alloc_func( u32 size, const char* file, u32 line );
extern inline int Sign( const f32 a, const f32 threshold );
extern inline f32 Abs( const f32 a );
extern inline bool Eq( f32 a, f32 b, f32 e );
extern inline f32 RandomValue( f32 a, f32 b ); 
extern inline f32 Deg2Rad( const f32 a );
extern inline f32 Rad2Deg( const f32 a );


u32 ReadFile( char **pBuffer, const char *pFile ) {
    check( pFile, "In ReadFile : given file name is uninitialized!\n" );

    // if buffer exists, destroy it
    if( *pBuffer )
        DEL_PTR( *pBuffer, sizeof( *pBuffer ) );

    FILE *file;
    u32 file_size = 0;

    file = fopen( pFile, "r" );

    check( file, "Couldn't open file \"%s\".\n", pFile );

    // Get file size in bytes
    fseek( file, 0L, SEEK_END );
    file_size = ftell( file );
    fseek( file, 0L, SEEK_SET );

    // allocate string and copy file contents in it
    *pBuffer = byte_alloc( file_size + 1 );
    fread( *pBuffer, 1, file_size, file );

    fclose( file );

    return (file_size + 1);

error:
    fclose( file );
    return 0;
}

void GetTime( char *t, int t_size,  const char *fmt ) {
    if( !t || !fmt ) return;

    time_t ti = time( NULL );
    struct tm* lt = localtime( &ti );
    strftime( t, t_size, fmt, lt );
}

