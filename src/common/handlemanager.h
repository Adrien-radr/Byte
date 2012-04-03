#ifndef BYTE_HANDLEMANAGER_HPP
#define BYTE_HANDLEMANAGER_HPP

#include "common.h"

typedef struct {
    union {
        void*   data;
        int     handle;
    };
    int     next_free_index;
    bool    used;
    bool    manage;
} Handle;

/// Container using indices and dynamic directing to store single entries 
/// containing a unique sort of data. The user of the HM provides the Data he wants
/// to store and the HM returns him a handle(u32) directing him to it.
/// The HM does not care about memory managment and this should be done elsewhere.
/// It is just a dynamic storage. Nothing is allocated or free'd here.
typedef struct {
    Handle  *mHandles;              ///< Handle entries
    u32     mCount;                 ///< Handle count
    u32     mMaxIndex;              ///< Farthest index used in array
    u32     mSize;                  ///< Handle manager capacity
    int     mFirstFreeIndex;        ///< First free index that is writable
} HandleManager;

/// Initialize and return a new Handle Manager with an initial pCount size
HandleManager *HandleManager_init( u32 pSize );

/// Destroy the given Handle Manager after having destroyed every used handle (with FreeFunc)
void HandleManager_destroy( HandleManager *pHm );

/// Add a new handle entry (with pointer data) in the given handle manager and returns its index
/// @param manage : true if the data must be destroyed at manager destruction
/// @param data_size : sizeof the given data, only if managment wanted. else, set to 0
/// @return : the handle of new data if successful. -1 if error.
int  HandleManager_addData( HandleManager *pHm, void *pData, bool manage, u32 data_size );

/// Add a new handle entry (with u32 data) in the given handle manager and returns its index
/// @return : the handle of new data if successful. -1 if error.
int  HandleManager_addHandle( HandleManager *pHm, u32 pData );

/// Remove a handle in given handle manager
/// @param pHandle : the index of the handle to be removed
void HandleManager_remove( HandleManager *pHm, u32 pHandle );

/// Returns the data contained in the given handle manager at the given index
/// @return : the data or NULL if any error occured
void *HandleManager_getData( HandleManager *pHm, u32 pHandle );

/// Returns the handle contained in the given handle manager at a given index
int HandleManager_getHandle( HandleManager *pHm, u32 pHandle );

/// Returns whether or not the data at the given index is used or no
bool HandleManager_isUsed( HandleManager *pHm, u32 pHandle );

/// Clear the handle manager content
void HandleManager_clear( HandleManager *pHm );

/// Prints all the content of the handle manager
void HandleManager_print( HandleManager *pHm );

#endif // BYTE_HANDLEMANAGER

