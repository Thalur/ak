/**
 * Global resource files and categories definition to be used throughout the project.
 */
#ifndef AK_RESOURCELIST_H_INCLUDED
#define AK_RESOURCELIST_H_INCLUDED

#include "client/main/ResourceManager.h"

/**
 * The resource categories. Game states can require any combination of
 * categories as their data. STARTUP and COMMON are always included.
 */
enum ECategory {
   CAT_STARTUP, // Resources required for the splash screen and loading status (must be 0)
   CAT_COMMON,  // Files used by all game states (in addition to startup files)
   CAT_TEST,    // OGLTest main state files
   CAT_NUM_CATEGORIES
};

/**
 * List of all files present in resources to be used directly in the code.
 */
enum EFile {
   // STARTUP files
   F_FONT_DUNE3DLARGE, F_FONT_DUNE3DSMALL, F_DIALOG_BACK_SUNKEN, F_DIALOG_BACK_OUTSTANDING,
   F_STARTUP_LOADTEXT, F_STARTUP_PROGRESS_EMPTY, F_STARTUP_PROGRESS_FULL,
   F_STARTUP_MUSIC, F_STARTUP_SOUND, F_STARTUP_DISABLED, F_STARTUP_BACKGROUND,

   // COMMON files
   F_FONT_DUNESMALLCAPS, F_FONT_DUNEBLACK, F_FONT_DUNEHOUSE,
   F_DIALOG_ARROW_UP, F_DIALOG_ARROW_UP_PRESSED, F_DIALOG_ARROW_DOWN,
   F_DIALOG_ARROW_DOWN_PRESSED, F_DIALOG_SLIDER, F_DIALOG_SLIDER_BAR,

   // All further resource files (can be in multiple categories)
   F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112,

   F_NUM_RESOURCE_FILES
};

#endif // AK_RESOURCELIST_H_INCLUDED
