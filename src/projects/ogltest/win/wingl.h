/**
 *
 */
#ifndef AK_WINGL_INCLUDED
#define AK_WINGL_INCLUDED

#include "common/cabinet/Cabinet.h"
#include <string>

namespace WinGL {

bool SetupOpenGL(int& argc, char** argv,const std::string& aWindowTitle);

void RunGame(TCabinetPtr& aCabinet);

} // namespace WinGL

#endif // AK_WINGL_INCLUDED
