/**
 * Main application entry point for the Posix systems
 */
#include "common/log/log.h"
#include "OGLTest.h"

namespace Client
{
extern void Run(TAppPtr aAppPtr, int argc, char** argv);
}
extern Client::TAppPtr CreateApplication();

int main(int argc, char** argv)
{
   Client::TAppPtr app = CreateApplication();
   NLogging::InitLogFile(app->AppName(), "log.txt");
   Client::Run(app, argc, argv);
   NLogging::FinishLogger();
}