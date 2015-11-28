/**
 * Main application entry point for the Windows binaries
 */
#include "common/log/log.h"
#include "wingl.h"


int main(int argc, char** argv)
{
   NLogging::InitLogFile("AK-multitest1", "log.txt");
   LOG_INFO("main() called.");

   if (WinGL::SetupOpenGL(argc, argv, "Multitest1 Main Window ßäöü")) {
      WinGL::RunGame();
   }
   LOG_INFO("LEAVING main.");
}
