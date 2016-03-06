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
   const TNames names { "test.ak" };
   return names;
}

/**
 * A list of all cabinets required by the program. Order is important.
 */
COGLTest::TNames COGLTest::RequiredCabinets()
{
   return TNames();
}

/**
 * A list of additional cabinets used by the program. Order is important.
 * Can include wildcards to account for future patch files.
 */
COGLTest::TNames COGLTest::OptionalCabinets()
{
   return TNames();
}


/**
 * A list of all resource files that can be referenced in the source code.
 */
Client::TResourceFiles COGLTest::GetResourceFiles()
{
   using Type = Client::EFileType;
   const Client::TResourceFiles resourceFiles = {
      std::make_tuple(F_ALPHA320X200, Type::GFX, "alpha320x200"),
      std::make_tuple(F_ALPHA32X32,   Type::GFX, "alpha32x32"),
      std::make_tuple(F_ICON,         Type::GFX, "icon"),
      std::make_tuple(F_INDEX32X32,   Type::GFX, "index32x32"),
      std::make_tuple(F_INDEX64_48,   Type::GFX, "index64x48"),
      std::make_tuple(F_INDEX184x112, Type::GFX, "index184x112")
   };
   return resourceFiles;
}

Client::TCategoryContent COGLTest::GetCategoryContent()
{
   const Client::TCategoryContent categoryContent = {
      { CAT_STARTUP, { // Splash screen and loading progress
           F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112
      } },
      { CAT_COMMON, { // For all categories
      } },
      { CAT_TEST, { // OGLTest just needs this one main data category

      } },
   };
   return categoryContent;
}
