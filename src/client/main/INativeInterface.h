/**
 * Interface for platform-dependent operations required by the Engine.
 */
#ifndef AK_NATIVE_INTERFACE_H_INCLUDED
#define AK_NATIVE_INTERFACE_H_INCLUDED

#include "common/cabinet/File.h"
#include <string>
#include <memory>

namespace Client
{

class INativeInterface
{
public:
   virtual ~INativeInterface() {}

   // An internal file is located inside the binaries (except for mobile)
   virtual TFilePtr GetInternalFile(const std::string& aFilename) = 0;

   // Resource files are located in the package (mobile) or in the app folder (posix)
   virtual TFilePtr GetResourceFile(const std::string& aFilename) = 0;
};

typedef std::shared_ptr<INativeInterface> TNativePtr;

} // namespace Client

#endif // AK_NATIVE_INTERFACE_H_INCLUDED
