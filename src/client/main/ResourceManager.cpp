/**
 *
 */
#include "client/main/ResourceManager.h"
#include "common/log/log.h"
#include <algorithm>

namespace Client {

std::vector<TResourceFileId> CResourceManager::GetListForResource(const TRequiredResources aCategories)
{
   LOG_METHOD();
   std::vector<TResourceFileId> result;
   for (TResourceCategory cat = 0; cat < aCategories.size(); cat++) {
      if (aCategories[cat]) {
         if (!iCategoryCached[cat]) {
            CacheCategory(cat);
         }
         const std::vector<TResourceFileId>& files = iCategories[cat];
         LOG_DEBUG("Found %d files in category %u", files.size(), cat);
         result.insert(result.end(), files.begin(), files.end());
      }
   }
   std::sort(result.begin(), result.end());
   std::unique(result.begin(), result.end());
   return result;
}

std::pair<bool, TSize> CResourceManager::FindResourceId(const std::string &aResource)
{
   LOG_METHOD();
   for (auto file : iFiles) {
      if (std::get<2>(file) == aResource) {
         return std::make_pair(true, std::get<0>(file));
      }
   }
   return std::make_pair(false, 0);
}

void CResourceManager::CacheCategory(const TResourceCategory aCategory)
{
   LOG_PARAMS("%u", aCategory);
   std::vector<TResourceFileId> addedResources;
   for (const TResourceFileId file : iCategories[aCategory]) {
      const EFileType type = std::get<1>(iFiles[file]);
      if (type == EFileType::FONT) {
         // Font image files are added when the fonts are loaded
      } else if (type == EFileType::GFX) {
         // ... (Check if gfx is an animation file, if yes add files that are not referenced
      }
   }
   iCategories[aCategory].insert(iCategories[aCategory].end(),
                                 std::make_move_iterator(addedResources.begin()),
                                 std::make_move_iterator(addedResources.end()));
   iCategoryCached[aCategory] = true;
}

bool CResourceManager::IsResourceSubset(const TRequiredResources aSubset, const TRequiredResources aSuperset)
{
   const std::vector<TResourceFileId> subset = GetListForResource(aSubset);
   if (subset.empty()) {
      return true;
   }
   const std::vector<TResourceFileId> superset = GetListForResource(aSuperset);
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

/*TResourceFiles CResourceManager::GetResourceFiles(const TRequiredResources aCategories)
{
   LOG_PARAMS("%u", aCategories.to_ulong());
   const std::list<TResourceFileId> files = GetListForResource(aCategories);
   TResourceFiles result;
   for (TResourceFileId fileId : files) {
      result.push_back(iFiles[fileId]);
   }
   return result;
}*/

std::vector<std::pair<TSize, std::string>> CResourceManager::GetFileList() const
{
   LOG_METHOD();
   std::vector<std::pair<TSize, std::string>> result;
   for (const TResourceFile& file : iFiles) {
      LOG_VERBOSE("List entry: %d, %s", (int)std::get<0>(file), std::get<2>(file).c_str());
      result.emplace_back(std::get<0>(file), std::get<2>(file));
   }
   return result;
}

std::vector<std::pair<TSize, std::string>> CResourceManager::GetStartupFileList() const
{
   LOG_METHOD();
   const TResourceCategory startupCategory = 0;
   const auto it = iCategories.find(startupCategory);
   std::vector<std::pair<TSize, std::string>> result;
   for (const TResourceFileId id : it->second) {
      const TResourceFile& file = iFiles[id];
      LOG_VERBOSE("Startup entry: %d, %s", (int)std::get<0>(file), std::get<2>(file).c_str());
      result.emplace_back(std::get<0>(file), std::get<2>(file));
   }
   return result;
}

TFileList CResourceManager::GetFileList(const TRequiredResources aCategories, const EFileType aFileType)
{
   LOG_PARAMS("categories %s, type %d", aCategories.to_string().c_str(), aFileType);
   const std::vector<TResourceFileId> ids = GetListForResource(aCategories);
   TFileList result;
   for (const TResourceFileId id : ids) {
      const TResourceFile& file = iFiles[id];
      if (std::get<1>(file) == aFileType) {
         LOG_VERBOSE("List entry: %d, %s", (int)std::get<0>(file), std::get<2>(file).c_str());
         result.emplace_back(std::get<0>(file), std::get<2>(file));
      }
   }
   return result;
}

TResourceFileId CResourceManager::GetResourceId(const std::string& aResource)
{
   LOG_METHOD();
   std::pair<bool, TSize> found = FindResourceId(aResource);
   if (found.first) {
      return static_cast<TResourceFileId>(found.second);
   }
   TResourceFileId id = static_cast<TResourceFileId>(iFiles.size());
   LOG_DEBUG("Adding %s as %u", aResource.c_str(), id);
   iFiles.emplace_back(id, EFileType::GFX, aResource);
   iCategories[0].push_back(id);
   return id;
}

} // namespace Client
