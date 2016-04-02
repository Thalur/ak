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

   // test files
   F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112,

   // orc files
   F_ORC_00, F_ORC_01, F_ORC_02, F_ORC_03, F_ORC_04, F_ORC_05, F_ORC_06, F_ORC_07,
   F_ORC_08, F_ORC_09, F_ORC_10, F_ORC_11, F_ORC_12, F_ORC_13, F_ORC_14, F_ORC_15,
   F_ORC_16, F_ORC_17, F_ORC_18, F_ORC_19, F_ORC_20, F_ORC_21, F_ORC_22, F_ORC_23,
   F_ORC_24, F_ORC_25, F_ORC_26, F_ORC_27, F_ORC_28, F_ORC_29, F_ORC_30, F_ORC_31,
   F_ORC_32, F_ORC_33, F_ORC_34, F_ORC_35, F_ORC_36, F_ORC_37, F_ORC_38, F_ORC_39,
   F_ORC_40, F_ORC_41, F_ORC_42, F_ORC_43, F_ORC_44, F_ORC_45, F_ORC_46, F_ORC_47,
   F_ORC_48, F_ORC_49, F_ORC_50, F_ORC_51, F_ORC_52, F_ORC_53, F_ORC_54, F_ORC_55,
   F_ORC_56, F_ORC_57, F_ORC_58, F_ORC_59, F_ORC_60, F_ORC_61, F_ORC_62, F_ORC_63,
   F_ORC_64, F_ORC_65, F_ORC_66, F_ORC_67, F_ORC_68, F_ORC_69, F_ORC_70, F_ORC_71,

   F_NUM_RESOURCE_FILES
};

#endif // AK_RESOURCELIST_H_INCLUDED
