/**
 * Main application entry point for the Windows binaries
 */
#include "common/log/log.h"
#include "common/cabinet/MemoryFile.h"
#include "common/cabinet/Cabinet.h"
#include "wingl.h"

#define UNICODE
#include <windows.h>

namespace {

TMemoryFilePtr LoadResourceFile(uint16_t aName)
{
   HRSRC rc = ::FindResource(NULL, MAKEINTRESOURCE(aName), RT_RCDATA);
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
   LOG_ERROR("Could not load internal resource file %u.", aName);
   return TMemoryFilePtr();
}

}

int main(int argc, char** argv)
{
   NLogging::InitLogFile("AK-multitest1", "log.txt");
   LOG_INFO("main() called.");
   TFilePtr filePtr = LoadResourceFile(9999);
   if (filePtr) {
      TCabinetPtr cabinet = CCabinet::Open(filePtr);
      if (cabinet) {
         LOG_INFO("Cabinet successfully opened");
         /*TMemoryFilePtr file = cabinet->ReadFileByName("icon.png");
         if (file) {
            CMemoryFile::TStringStreamPtr stream = file->CreateStream();
            char buffer[100];
            stream->getline(buffer, 99);
            LOG_INFO("DATA: %s", buffer);
         } else {
            LOG_ERROR("Could not read CMakeLists.txt");
         }*/
      } else {
         LOG_ERROR("Could not open the cabinet");
      }

      if (WinGL::SetupOpenGL(argc, argv, "Multitest1 Main Window ßäöü")) {
         WinGL::RunGame(cabinet);
      }
   }
   LOG_INFO("LEAVING main.");
   NLogging::FinishLogger();
}
