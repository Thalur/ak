/**
 *
 */
#include "CabManager.h"
#include "common/log/log.h"
#include <algorithm>


namespace {

bool CmpCabinetsByPatch(const TCabinetPtr& aFirst, const TCabinetPtr& aSecond)
{
   return aFirst->GetPatchCycle() < aSecond->GetPatchCycle();
}

bool CmpFileNameIndex(const CCabManager::TFileEntry& aFirst, const std::string& aSecond)
{
   return std::get<0>(aFirst) < aSecond;
}

}


bool CCabManager::Init(std::vector<TCabinetPtr> aCabinets, const TIndexData& aIndexValues, TSize aMaxIndex)
{
   LOG_METHOD();
   iCabinets.swap(aCabinets);
   TSize size = iCabinets.size();
   if (size == 0) {
      return false;
   }
   if (size > 1) {
      std::stable_sort(iCabinets.begin(), iCabinets.end(), CmpCabinetsByPatch);
   }

   // Create the file prefix index
   for (uint8_t i = static_cast<uint8_t>(size); i > 0; i--) {
      MergeIntoFileList(i-1, iCabinets[i-1]->GetFilePrefixList());
   }

   // Create the quick access index
   iResourceIndex.resize(aMaxIndex);
   for (auto& resourceEntry : aIndexValues) {
      std::vector<TFileEntry>::iterator it = std::lower_bound(iFileIndex.begin(), iFileIndex.end(),
         resourceEntry.second, CmpFileNameIndex);
      if (it != iFileIndex.end()) {
         iResourceIndex[resourceEntry.first] = TIndexEntry(std::get<1>(*it), std::get<2>(*it));
      } else LOG_ERROR("File not found in cabinet index: %s", resourceEntry.second.c_str());
   }
   return true;
}

void CCabManager::MergeIntoFileList(uint8_t aCabinetIndex, const std::vector<TEntryIndex>& aEntries)
{
   auto itIndex = iFileIndex.begin();
   auto itData = aEntries.begin();
   const auto itDataEnd = aEntries.end();
   bool bIndexEnd = itIndex == iFileIndex.end();
   while (itData != itDataEnd) {
      int cmp = bIndexEnd ? 1 : std::get<0>(*itIndex).compare(itData->first);
      if (cmp < 0) { // Leave index element at its position
         if (++itIndex == iFileIndex.end()) {
            bIndexEnd = true;
         }
      } else if (cmp > 0) { // Insert the data string before the current position
         itIndex = iFileIndex.emplace(itIndex, itData->first, aCabinetIndex, itData->second) + 1;
         if (!bIndexEnd && itIndex == iFileIndex.end()) {
            bIndexEnd = true;
         }
         itData++;
      } else {
         LOG_DEBUG("Ignoring file entry %s because file is already indexed", std::get<0>(*itIndex).c_str());
         itData++;
      }
   }
}

TMemoryFilePtr CCabManager::GetFile(TSize aIndex) const
{
   if (aIndex < iResourceIndex.size()) {
      const TIndexEntry& entry = iResourceIndex[aIndex];
      LOG_DEBUG("GetFile: %" PRIuS " (%u, %" PRIuS ")", aIndex, entry.first, entry.second);
      TCabinetPtr cab = iCabinets[entry.first];
      if (cab) {
         return cab->ReadFileByIndex(entry.second);
      } else {
         LOG_ERROR("Invalid cabinet handle");
      }
   }
   LOG_ERROR("Invalid index %" PRIuS, aIndex);
   return TMemoryFilePtr();
}

TMemoryFilePtr CCabManager::GetFile(const std::string& aFilePrefix) const
{
   LOG_DEBUG("GetFile: %s", aFilePrefix.c_str());
   const std::vector<TFileEntry>::const_iterator it = std::lower_bound(iFileIndex.begin(), iFileIndex.end(),
         aFilePrefix, CmpFileNameIndex);
   if (it != iFileIndex.end()) {
      LOG_DEBUG("Found file %s: %u, %u", aFilePrefix.c_str(), std::get<1>(*it), std::get<2>(*it));
      return iCabinets[std::get<1>(*it)]->ReadFileByIndex(std::get<2>(*it));
   }
   LOG_ERROR("Invalid prefix %s", aFilePrefix.c_str());
   return TMemoryFilePtr();
}

