/**
 *
 */
#ifndef AK_RESOURCE_MANAGER_H_INCLUDED
#define AK_RESOURCE_MANAGER_H_INCLUDED

#include "common/types.h"
#include <vector>
#include <string>
#include <map>
#include <bitset>
#include <list>

namespace Client
{

/**
 * File type specification so we do not depend on file name suffixes
 */
enum class EFileType {
   UNKNOWN, // unspecified - do not use
   TEXT, // normal text file, will not be loaded
   FONT, // font definition file, will load the font (and the specified font image file)
   GFX, // image file, will be loaded
   SFX, // sound file, will be loaded
   MUSIC, // music file, will not be loaded (to be streamed)
   OTHER, // none of the above, will not be loaded
};

/**
 * A resource file definition consists of an integer ID used to refer to the file
 * in the source code, the file type, and the string file name in the data cabinets.
 */
using TResourceFileId = uint32_t;
using TResourceFile = std::tuple<TResourceFileId, EFileType, std::string>;
using TResourceFiles = std::vector<TResourceFile>;

/**
 * A resource category is a collection of files that need to be loaded.
 * A game state may require one or more categories to work.
 * This is a bit field, allowing up to 32 resource categories.
 */
using TResourceCategory = uint32_t;
using TRequiredResources = std::bitset<32>;

/**
 * Category content defines which files are used in which categories.
 */
using TCategoryContent = std::map<TResourceCategory, std::vector<TResourceFileId>>;

/**
 * List of files of a specific file type that are required for a specific category combination.
 */
using TFileList = std::vector<std::pair<TResourceFileId, std::string>>;


class CResourceManager
{
public:
   CResourceManager(TResourceFiles aKnownFiles, TCategoryContent aCategoryContent)
    : iFiles(std::move(aKnownFiles))
    , iCategories(std::move(aCategoryContent))
    , iCategoryCached(GetMaxIndex(iCategories))
   {}

   static TSize GetMaxIndex(const TCategoryContent& aCategoryContent)
   {
      if (aCategoryContent.empty()) {
         return 0;
      }
      return aCategoryContent.rbegin()->first + 1;
   }

   TResourceFiles GetResourceFiles(TRequiredResources aCategories);

   std::vector<std::pair<TSize, std::string>> GetFileList() const;
   TFileList GetFileList(TRequiredResources aCategories, EFileType aFileType);

   // Convenience method
   bool IsResourceSubset(TRequiredResources aSubset, TRequiredResources aSuperset);

private:
   std::list<TResourceFileId> GetListForResource(TRequiredResources aCategories);

   TResourceFiles iFiles;
   TCategoryContent iCategories;
   std::vector<bool> iCategoryCached; // iCategories for this category has been updated with internal dependencies
};

} // namespace Client

#endif // AK_RESOURCE_MANAGER_H_INCLUDED
