/**
 *
 */
#include "OGLTest.h"

const std::string COGLTest::iAppName = "OGLTest by AK";

Client::TAppPtr CreateApplication()
{
   return std::make_shared<COGLTest>();
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

Client::TGameStatePtr COGLTest::GameState()
{
   return Client::TGameStatePtr();
}
