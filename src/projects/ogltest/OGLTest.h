/**
 *
 */
#ifndef AK_OGLTEST_H_INCLUDED
#define AK_OGLTEST_H_INCLUDED

#include "client/main/IAppInterface.h"


class COGLTest : public Client::IAppInterface
{
public:
   COGLTest();
   virtual ~COGLTest() {}

   // IAppInterface implementation
   virtual std::string AppName() { return iAppName; }
   virtual TNames InternalFiles();
   virtual TNames ResourceFiles();
   virtual TGameStatePtr GameState();

private:
   static const std::string iAppName;

   TGameStatePtr iGameState;
};

#endif // AK_OGLTEST_H_INCLUDED
