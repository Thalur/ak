/**
 *
 */
#include "NativePosix.h"
#include "common/log/log.h"
#include "common/cabinet/MemoryFile.h"

#ifdef AK_SYSTEM_WINDOWS
#define UNICODE
#include <windows.h>
#endif

namespace Client
{

#ifdef AK_SYSTEM_WINDOWS
/**
 * WINDOWS VERSION
 * Load a resource file from the executable.
 * This does not support filenames, so we just make sure it's correct
 */
TFilePtr CNativePosix::GetInternalFile(const std::string& aFilename)
{
   if (aFilename != "test.ak") {
      LOG_ERROR("Unknown resource file %s", aFilename.c_str());
      return TFilePtr();
   }
   HRSRC rc = ::FindResource(NULL, MAKEINTRESOURCE(9999), RT_RCDATA);
   if (rc != NULL) {
      HGLOBAL rcData = ::LoadResource(NULL, rc);
      if (rcData != NULL) {
         DWORD size = ::SizeofResource(NULL, rc);
         if (size > 0) {
            const char* data = static_cast<const char*>(::LockResource(rcData));
            if (data != NULL) {
               TFileData fileData(size);
               std::copy(data, data+size, fileData.begin());
               return std::make_shared<CMemoryFile>(std::move(fileData));
            }
         }
      }
   }
   LOG_ERROR("Could not load internal resource file %s.", aFilename.c_str());
   return TFilePtr();
}
#else
TFilePtr CNativePosix::GetInternalFile(const std::string& aFilename)
{
   return GetResourceFile(aFilename);
}
#endif

TFilePtr CNativePosix::GetResourceFile(const std::string& aFilename)
{
   // ...
   return TFilePtr();
}

} // namespace Client
