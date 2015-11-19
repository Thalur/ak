/**
 *
 */
#ifndef AK_WINGL_INCLUDED
#define AK_WINGL_INCLUDED

#include <string>

namespace WinGL {

bool SetupOpenGL(int& argc, char** argv,const std::string& aWindowTitle);

void RunGame();

} // namespace WinGL

#endif // AK_WINGL_INCLUDED
