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
   NLogging::InitLogFile(app->AppName(), "log.txt", ELogLevel::EDEBUG, ELogLevel::EINFO);
   Client::Run(app, argc, argv);
   NLogging::FinishLogger();
}

// Propagate access to the internal resources to the client implementation
extern const std::string internal_cabinet_name;
const std::string internal_cabinet_name { "test.ak" };
#ifdef AK_SYSTEM_WINDOWS
extern const int internal_cabinet_resource_id;
const int internal_cabinet_resource_id = 9999;
#elif defined(AK_SYSTEM_LINUX)
extern char _binary_test_ak_start;
extern char _binary_test_ak_end;
extern const char* internal_cabinet_start_ptr;
extern const char* internal_cabinet_start_ptr;
const char* internal_cabinet_start_ptr = &_binary_test_ak_start;
const char* internal_cabinet_end_ptr = &_binary_test_ak_end;
#else // OSX
extern const char* internal_cabinet_section_name;
const char* internal_cabinet_section_name = "core_resources";
#endif
