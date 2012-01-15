#ifndef BYTE_HANDLEMANAGER_HPP
#define BYTE_HANDLEMANAGER_HPP

#include "common.h"

typedef struct {
    void*   data;
    int     next_free_index;
    bool    used;
} Handle;

/// Container using indices and dynamic directing to store single entries 
/// containing a unique sort of data. The user of the HM provides the Data he wants
/// to store and the HM returns him a handle(u32) directing him to it.
/// The HM does not care about memory managment and this should be done elsewhere.
/// It is just a dynamic storage. Nothing is allocated or free'd here.
typedef struct {
    Handle  *mHandles;              ///< Handle entries
    u32     mCount;                 ///< Handle count
    u32     mSize;                  ///< Handle manager capacity
    int     mFirstFreeIndex;        ///< First free index that is writable
} HandleManager;

/// Initialize and return a new Handle Manager with an initial pCount size
HandleManager *HandleManager_init( u32 pSize );

/// Destroy the given Handle Manager after having destroyed every used handle (with FreeFunc)
void HandleManager_destroy( HandleManager *pHm );

/// Add a new handle entry in the given handle manager and returns its index
/// @return : the handle of new data if successful. -1 if error.
int  HandleManager_add( HandleManager *pHm, void *pData );


/// Remove a handle in given handle manager
/// @param pHandle : the index of the handle to be removed
void HandleManager_remove( HandleManager *pHm, u32 pHandle );

/// Returns the data contained in the given handle manager at the given index
/// @return : the data or NULL if any error occured
void *HandleManager_get( HandleManager *pHm, u32 pHandle );

void HandleManager_print( HandleManager *pHm );
/*
u32 revHandleManagerAdd( revHandleManager* pHM, void* pData );
void revHandleManagerRemove( revHandleManager* pHM, u32 pData );
void* revHandleManagerGet( revHandleManager* pHM, u32 pHandle );

void printHM( revStrManager* hm);
*/

#endif // BYTE_HANDLEMANAGER

