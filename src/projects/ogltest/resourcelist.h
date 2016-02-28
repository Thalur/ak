/**
 * Global resource files and categories definition to be used throughout the project.
 */
#ifndef AK_RESOURCELIST_H_INCLUDED
#define AK_RESOURCELIST_H_INCLUDED

#include "client/resources.h"

/**
 * The resource categories. Game states can require any combination of
 * categories as their data. STARTUP and COMMON are always included.
 */
enum class ECategory {
   STARTUP, // Resources required for the splash screen and loading status
   COMMON,  // Files used by all game states (in addition to startup files)
   TEST,    // OGLTest main state files
   NUM_CATEGORIES
};

/**
 * List of all files present in resources to be used directly in the code.
 */
enum class EFile {
   // STARTUP files
   ALPHA32X32, ALPHA320X200, ICON, INDEX32X32, INDEX64_48, INDEX184x112,

   // COMMON files
   // ...

   // All further resource files (can be in multiple categories)
   // ...

   NUM_RESOURCE_FILES
};

TResourceFiles<EFile> GetResourceFiles(const TRequiredResources<ECategory>& aCategories);

#endif // AK_RESOURCELIST_H_INCLUDED
