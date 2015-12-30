/**
 *
 */
#ifndef AK_NATIVE_POSIX_H_INCLUDED
#define AK_NATIVE_POSIX_H_INCLUDED

#include "client/gfx/IEngine.h"
#include "IAppInterface.h"

namespace Client
{

class CNativePosix : public INativeCallback
{
public:
   CNativePosix(std::string&& aAppPath) : iAppPath(std::move(aAppPath))
   {}
   virtual ~CNativePosix() {}

   // INativeCallback implementation
   virtual TFilePtr GetInternalFile(const std::string& aFilename);
   virtual TFilePtr GetResourceFile(const std::string& aFilename);

   // Run the main app loop
   void RunGame();

private:
   std::string iAppPath;
};

} // namespace Client

#endif // AK_NATIVE_POSIX_H_INCLUDED
