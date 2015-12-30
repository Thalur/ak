/**
 *
 */
#include "OGLTest.h"

const std::string COGLTest::iAppName = "OGLTest by AK";

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

Client::TGameStatePtr COGLTest::GameState()
{
   return Client::TGameStatePtr();
}
