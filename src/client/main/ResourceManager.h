/**
 * Resource management definitions and types.
 *
 * Loading resources happens at 5 different stages in the application lifecycle:
 * 1. Files for the initial splash screen (usually only images, from internal).
 * 2. Files necessary to show the extended splash screen, e.g. loading animations,
 *    and files used in error situations like missing required assets (from internal).
 * 3. Data used for the initial / current game state, loaded while load screen shows.
 * 4. Data for other game states, can be loaded in the background.
 * 5. Lazily loaded data, only loaded when the resource is actually used.
 */
#ifndef AK_RESOURCE_MANAGER_H_INCLUDED
#define AK_RESOURCE_MANAGER_H_INCLUDED

#include "common/types.h"
#include <vector>
#include <string>
#include <map>
#include <bitset>


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

   /**
    * Returns the list of files (resource index and file name)
    * for all known resource files. This is used to build the cabinet file index.
    */
   std::vector<std::pair<TSize, std::string>> GetFileList() const;
   std::vector<std::pair<TSize, std::string>> GetStartupFileList() const;

   /**
    * Returns the list of files (resource index and file name)
    * for all resources of the specified type for the specified categorie combination.
    */
   TFileList GetFileList(TRequiredResources aCategories, EFileType aFileType);

   /**
    * Retrieve the resource identifier for the given resource name,
    * or, if it does not exist yet, add it to the known resources.
    */
   TResourceFileId GetResourceId(const std::string &aResource);

   const std::string* GetResourceFileName(TResourceFileId aId);

   // Convenience method
   bool IsResourceSubset(TRequiredResources aSubset, TRequiredResources aSuperset);

private:
   std::vector<TResourceFileId> GetListForResource(TRequiredResources aCategories);
   std::pair<bool, TSize> FindResourceId(const std::string& aResource);

   /**
    * Add referenced files from animation or font files to the category.
    */
   void CacheCategory(TResourceCategory aCategory);

   TResourceFiles iFiles;
   TCategoryContent iCategories;
   std::vector<bool> iCategoryCached; // iCategories for this category has been updated with internal dependencies
};

} // namespace Client

#endif // AK_RESOURCE_MANAGER_H_INCLUDED
