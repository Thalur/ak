/**
 *
 */
#include "NativePosix.h"
#include "common/log/log.h"
#include "common/cabinet/MemoryFile.h"
#include "common/cabinet/PosixFile.h"

// Internal resource parameters supplied by the main app
#ifdef AK_SYSTEM_WINDOWS
#define UNICODE
#include <windows.h>
extern const int internal_cabinet_resource_id;
#else
extern const char* internal_cabinet_start_ptr;
extern const char* internal_cabinet_end_ptr;
#endif
extern const char* internal_cabinet_name;

namespace Client
{

#ifdef AK_SYSTEM_WINDOWS
/**
 * WINDOWS VERSION
 * Load a resource file from the executable.
 * The supplied filename must match the supported resource file from the application.
 */
TFilePtr CNativePosix::GetInternalFile(const std::string& aFilename)
{
   LOG_METHOD();
   if (aFilename != std::string(internal_cabinet_name)) {
      LOG_ERROR("Unknown resource file %s", aFilename.c_str());
      return TFilePtr();
   }
   HRSRC rc = ::FindResource(NULL, MAKEINTRESOURCE(internal_cabinet_resource_id), RT_RCDATA);
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
/**
 * LINUX/OSX VERSION
 * Load an embedded object resource from the executable.
 * The supplied filename must match the supported resource file from the application.
 */
TFilePtr CNativePosix::GetInternalFile(const std::string& aFilename)
{
   LOG_METHOD();
   if (aFilename != std::string(internal_cabinet_name)) {
      LOG_ERROR("Unknown resource file %s", aFilename.c_str());
      return TFilePtr();
   }
   TFileData fileData;
   for (const char* data = internal_cabinet_start_ptr; data != internal_cabinet_end_ptr; data++) {
      fileData.push_back(*data);
   }
   return std::make_shared<CMemoryFile>(std::move(fileData));
}
#endif

TFilePtr CNativePosix::GetResourceFile(const std::string& aFilename)
{
   return CPosixFile::OpenExistingFile(iAppPath + aFilename, false);
}

} // namespace Client
