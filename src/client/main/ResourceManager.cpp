/**
 *
 */
#include "client/main/ResourceManager.h"

namespace Client {

std::list<TResourceFileId> CResourceManager::GetFileList(const TRequiredResources aCategories)
{
   std::list<TResourceFileId> result;
   for (TResourceCategory cat = 0; cat < aCategories.size(); cat++) {
      if (!iCategoryCached[cat]) {
         // ... (add referenced files from animation or font files)
      }
      const auto& files = iCategories[cat];
      result.insert(result.begin(), files.begin(), files.end());
   }
   result.sort();
   result.unique();
   return result;
}

bool CResourceManager::IsResourceSubset(const TRequiredResources aSubset, const TRequiredResources aSuperset)
{
   const std::list<TResourceFileId> subset = GetFileList(aSubset);
   if (subset.empty()) {
      return true;
   }
   const std::list<TResourceFileId> superset = GetFileList(aSuperset);
   auto subEnd = subset.end();
   auto superEnd = superset.end();
   for (auto subIt = subset.begin(), superIt = superset.begin(); superIt != superEnd ; ) {
      const TResourceFileId subFile = *subIt;
      const TResourceFileId superFile = *superIt;
      if (superFile < subFile) {
         superIt++;
      } else if (superFile == subFile) {
         superIt++;
         subIt++;
         if (subIt == subEnd) {
            return true;
         }
      } else break;
   }
   return false;

   //auto ContainedInSuperset = [&](const EResourceFile& value) {
   //   return std::find(superset.begin(), superset.end(), value) != superset.end();
   //};
   //subset.remove_if(ContainedInSuperset);
   //return subset.empty();
}

TResourceFiles CResourceManager::GetResourceFiles(const TRequiredResources aCategories)
{
   const std::list<TResourceFileId> files = GetFileList(aCategories);
   TResourceFiles result;
   for (TResourceFileId fileId : files) {
      result.push_back(iFiles[fileId]);
   }
   return result;
}

} // namespace Client
