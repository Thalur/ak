/**
 * Main application entry point for the Posix systems
 */
#include "common/log/log.h"
#include "OGLTest.h"

namespace Client
{

extern void Run(TAppPtr aAppPtr, int argc, char** argv);

}

int main(int argc, char** argv)
{
   std::shared_ptr<COGLTest> app = std::make_shared<COGLTest>();
   NLogging::InitLogFile(app->AppName(), "log.txt");
   Client::Run(app, argc, argv);
   NLogging::FinishLogger();
}
