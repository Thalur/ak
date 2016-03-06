/**
 *
 */
#ifndef AK_NATIVE_POSIX_H_INCLUDED
#define AK_NATIVE_POSIX_H_INCLUDED

#include "INativeInterface.h"

namespace Client
{

class CNativePosix : public INativeInterface
{
public:
   CNativePosix(std::string&& aAppPath) : iAppPath(std::move(aAppPath))
   {}
   virtual ~CNativePosix() {}

   // INativeInterface implementation
   virtual TFilePtr GetInternalFile(const std::string& aFilename) final;
   virtual TFilePtr GetResourceFile(const std::string& aFilename) final;

private:
   std::string iAppPath;
};

} // namespace Client

#endif // AK_NATIVE_POSIX_H_INCLUDED
