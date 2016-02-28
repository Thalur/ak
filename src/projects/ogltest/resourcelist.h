/**
 * Global resource files and categories definition to be used throughout the project.
 */
#ifndef AK_RESOURCELIST_H_INCLUDED
#define AK_RESOURCELIST_H_INCLUDED

#include "client/ResourceManager.h"

/**
 * The resource categories. Game states can require any combination of
 * categories as their data. STARTUP and COMMON are always included.
 */
enum ECategory {
   CAT_STARTUP, // Resources required for the splash screen and loading status
   CAT_COMMON,  // Files used by all game states (in addition to startup files)
   CAT_TEST,    // OGLTest main state files
   CAT_NUM_CATEGORIES
};

/**
 * List of all files present in resources to be used directly in the code.
 */
enum EFile {
   // STARTUP files
   F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112,

   // COMMON files
   // ...

   // All further resource files (can be in multiple categories)
   // ...

   F_NUM_RESOURCE_FILES
};

#endif // AK_RESOURCELIST_H_INCLUDED
