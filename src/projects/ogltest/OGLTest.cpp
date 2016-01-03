/**
 *
 */
#include "OGLTest.h"
#include "state/GameStateDialog.h"

const std::string COGLTest::iAppName = "OGLTest by AK";

Client::TAppPtr CreateApplication()
{
   return std::make_shared<COGLTest>();
}

COGLTest::COGLTest()
{
   //iGameState = std::make_shared<CGameStateDialog>();
}

COGLTest::TNames COGLTest::InternalFiles()
{
   TNames names;
   names.push_back("test.ak");
   return names;
}

COGLTest::TNames COGLTest::ResourceFiles()
{
   return TNames();
}

TGameStatePtr COGLTest::GameState()
{
   return TGameStatePtr();
}
