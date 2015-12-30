/**
 *
 */
#ifndef AK_OGLTEST_H_INCLUDED
#define AK_OGLTEST_H_INCLUDED

#include "client/main/IAppInterface.h"


class COGLTest : public Client::IAppInterface
{
public:
   virtual ~COGLTest() {}

   // IAppInterface implementation
   virtual std::string AppName() { return iAppName; }
   virtual TNames InternalFiles();
   virtual TNames ResourceFiles();
   virtual Client::TGameStatePtr GameState();

private:
   static const std::string iAppName;
};

#endif // AK_OGLTEST_H_INCLUDED
