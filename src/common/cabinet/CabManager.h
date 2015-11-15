/**
 *
 */
#ifndef AK_CABMANAGER_H_INCLUDED
#define AK_CABMANAGER_H_INCLUDED

#include "common/cabinet/Cabinet.h"
#include <tuple>

typedef std::vector<std::pair<TSize, std::string>> TIndexData;

class CCabManager
{
public:
   // Index for 1. iCabinets, 2. file number in cabinet
   typedef std::pair<uint8_t, TSize> TIndexEntry;
   // Filename cannot be const ref when using insert()
   typedef std::tuple<std::string, uint8_t, TSize> TFileEntry;

   bool Init(std::vector<TCabinetPtr> aCabinets, const TIndexData& aIndexValues, TSize aMaxIndex);

   TMemoryFilePtr GetFile(TSize aIndex) const;
   TMemoryFilePtr GetFile(const std::string& aFilePrefix) const;

private:
   void MergeIntoFileList(uint8_t aCabinetIndex, const std::vector<TEntryIndex>& aEntries);

   std::vector<TCabinetPtr> iCabinets;
   std::vector<TIndexEntry> iResourceIndex;
   std::vector<TFileEntry>  iFileIndex;
};

#endif // AK_CABMANAGER_H_INCLUDED
