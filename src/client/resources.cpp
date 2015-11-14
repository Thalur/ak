/**
 *
 */
#include "client/resources.h"
#include <tuple>
#include <list>

namespace {

// List of all known resource files
const TResourceFiles resourceFiles {
   std::make_tuple(FILE_FONTWHATEVER, FT_GFX, "gfx/fonts/fontwhatever"),
   std::make_tuple(FILE_UNIT257, FT_GFX, "gfx/units/unit257")
};

// List of files that belong to each category
const std::vector<std::vector<EResourceFile>> categoryFiles {
   {}, // GFXCAT_NONE
   // GFXCAT_DEFAULT:
   { FILE_FONTWHATEVER },
   // GFXCAT_INTRO:
   { FILE_UNIT257 },
   // GFXCAT_MENU:
   {}
};

std::list<EResourceFile> GetFileList(const TRequiredResources& aCategories)
{
   std::list<EResourceFile> files;
   for (EResourceCategory cat : aCategories) {
      files.insert(files.begin(), categoryFiles[cat].begin(), categoryFiles[cat].end());
   }
   files.sort();
   files.unique();
   return files;
}

} // namespace

bool IsResourceSubset(const TRequiredResources& aSubset, const TRequiredResources& aSuperset)
{
   std::list<EResourceFile> subset = GetFileList(aSubset);
   if (subset.empty()) return true;
   std::list<EResourceFile> superset = GetFileList(aSuperset);
   auto subEnd = subset.end();
   auto superEnd = superset.end();
   for (auto subIt = subset.begin(), superIt = superset.begin(); superIt != superEnd ; ) {
      EResourceFile subFile = *subIt;
      EResourceFile superFile = *superIt;
      if (superFile < subFile) {
         superIt++;
      } else if (superFile == subFile) {
         superIt++;
         subIt++;
         if (subIt == subEnd) return true;
      } else break;
   }
   return false;

   /*auto ContainedInSuperset = [&](const EResourceFile& value) { 
      return std::find(superset.begin(), superset.end(), value) != superset.end();
   };
   subset.remove_if(ContainedInSuperset);
   return subset.empty();*/
}

TResourceFiles GetResourceFiles(const TRequiredResources& aCategories)
{
   std::list<EResourceFile> files = GetFileList(aCategories);
   TResourceFiles result;
   for (EResourceFile file : files) {
      result.push_back(resourceFiles[file]);
   }
   return result;
}
