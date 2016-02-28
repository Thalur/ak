/**
 *
 */
#ifndef AK_RESOURCES_H_INCLUDED
#define AK_RESOURCES_H_INCLUDED

#include <vector>
#include <string>
#include <map>

/**
 * File type specification so we do not depend on file name suffixes
 */
enum EFileType {
   FT_UNKNOWN, // unspecified - do not use
   FT_TEXT, // normal text file, will not be loaded
   FT_FONT, // font definition file, will load the font (and the specified font image file)
   FT_GFX, // image file, will be loaded
   FT_SFX, // sound file, will be loaded
   FT_MUSIC, // music file, will not be loaded (to be streamed)
   FT_OTHER, // none of the above, will not be loaded
};

/**
 * A resource file definition consists of an integer ID used to refer to the file
 * in the source code, the file type, and the string file name in the data cabinets.
 */
using TResourceFile = std::tuple<uint32_t, EFileType, std::string>;
using TResourceFiles = std::vector<TResourceFile>;

/**
 * A resource category is a collection of files that need to be loaded.
 * A game state may require one or more categories to work.
 * This is a bit field, allowing up to 32 resource categories.
 */
using TRequiredResources = uint32_t;
using TCategoryContent = std::map<uint32_t, std::vector<uint32_t>>;


// Convenience functions
//bool IsResourceSubset(const TRequiredResources& aSubset, const TRequiredResources& aSuperset);


#endif // AK_RESOURCES_H_INCLUDED
