/**
 *
 */
#include "resourcelist.h"
#include "OGLTest.h"

/**
 * A list of all cabinets required for loading the CAT_STARTUP files.
 */
COGLTest::TNames COGLTest::InternalCabinets()
{
   const TNames names { "startup.ak" };
   return names;
}

/**
 * A list of all cabinets required by the program. Order is important.
 */
COGLTest::TNames COGLTest::RequiredCabinets()
{
   const TNames names { "test.ak" };
   return names;
}

/**
 * A list of additional cabinets used by the program. Order is important.
 * Can include wildcards to account for future patch files.
 */
COGLTest::TNames COGLTest::OptionalCabinets()
{
   const TNames names { "*patch*.ak" };
   return names;
}


/**
 * A list of all resource files that can be referenced in the source code.
 */
Client::TResourceFiles COGLTest::GetResourceFiles()
{
   using Type = Client::EFileType;
   const Client::TResourceFiles resourceFiles = {
      // Startup files
      std::make_tuple(F_FONT_DUNE3DLARGE,       Type::FONT, "fontdesc00"),
      std::make_tuple(F_FONT_DUNE3DSMALL,       Type::FONT, "fontdesc01"),
      std::make_tuple(F_FONT_DUNESMALLCAPS,     Type::FONT, "fontdesc02"),
      std::make_tuple(F_FONT_DUNEBLACK,         Type::FONT, "fontdesc03"),
      std::make_tuple(F_FONT_DUNEHOUSE,         Type::FONT, "fontdesc04"),
      std::make_tuple(F_STARTUP_LOADTEXT,       Type::GFX,  "startup00"),
      std::make_tuple(F_STARTUP_PROGRESS_EMPTY, Type::GFX,  "startup01"),
      std::make_tuple(F_STARTUP_PROGRESS_FULL,  Type::GFX,  "startup02"),
      std::make_tuple(F_STARTUP_MUSIC,          Type::GFX,  "startup03"),
      std::make_tuple(F_STARTUP_SOUND,          Type::GFX,  "startup04"),
      std::make_tuple(F_STARTUP_DISABLED,       Type::GFX,  "startup05"),
      std::make_tuple(F_STARTUP_BACKGROUND,     Type::GFX,  "startup06"),
      // Test files
      std::make_tuple(F_ALPHA320X200, Type::GFX,  "alpha320x200"),
      std::make_tuple(F_ALPHA32X32,   Type::GFX,  "alpha32x32"),
      std::make_tuple(F_ICON,         Type::GFX,  "icon"),
      std::make_tuple(F_INDEX32X32,   Type::GFX,  "index32x32"),
      std::make_tuple(F_INDEX64_48,   Type::GFX,  "index64x48"),
      std::make_tuple(F_INDEX184x112, Type::GFX,  "index184x112")
   };
   return resourceFiles;
}

Client::TCategoryContent COGLTest::GetCategoryContent()
{
   const Client::TCategoryContent categoryContent = {
      { CAT_STARTUP, { // Splash screen and loading progress
         F_FONT_DUNE3DLARGE, F_FONT_DUNE3DSMALL, F_FONT_DUNESMALLCAPS, F_FONT_DUNEBLACK, F_FONT_DUNEHOUSE,
         F_STARTUP_LOADTEXT, F_STARTUP_PROGRESS_EMPTY, F_STARTUP_PROGRESS_FULL,
         F_STARTUP_MUSIC, F_STARTUP_SOUND, F_STARTUP_DISABLED, F_STARTUP_BACKGROUND,
      } },
      { CAT_COMMON, { // For all categories
      } },
      { CAT_TEST, { // OGLTest just needs this one main data category
         F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112
      } },
   };
   return categoryContent;
}
