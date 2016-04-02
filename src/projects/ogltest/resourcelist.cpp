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
   const TNames names { "assets.ak" };
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

      ////////////////////
      /// Startup archive

      // Root directory
      std::make_tuple(F_FONT_DUNE3DLARGE,       Type::FONT, "fontdesc00"),
      std::make_tuple(F_FONT_DUNE3DSMALL,       Type::FONT, "fontdesc01"),
      std::make_tuple(F_STARTUP_LOADTEXT,       Type::GFX,  "startup00"),
      std::make_tuple(F_STARTUP_PROGRESS_EMPTY, Type::GFX,  "startup01"),
      std::make_tuple(F_STARTUP_PROGRESS_FULL,  Type::GFX,  "startup02"),
      std::make_tuple(F_STARTUP_MUSIC,          Type::GFX,  "startup03"),
      std::make_tuple(F_STARTUP_SOUND,          Type::GFX,  "startup04"),
      std::make_tuple(F_STARTUP_DISABLED,       Type::GFX,  "startup05"),
      std::make_tuple(F_STARTUP_BACKGROUND,     Type::GFX,  "startup06"),
      std::make_tuple(F_DIALOG_BACK_SUNKEN,     Type::GFX,  "dialog00"),
      std::make_tuple(F_DIALOG_BACK_OUTSTANDING,Type::GFX,  "dialog01"),

      ///////////////////
      /// Assets archive

      // Root directory
      std::make_tuple(F_FONT_DUNESMALLCAPS,     Type::FONT, "fontdesc02"),
      std::make_tuple(F_FONT_DUNEBLACK,         Type::FONT, "fontdesc03"),
      std::make_tuple(F_FONT_DUNEHOUSE,         Type::FONT, "fontdesc04"),
      std::make_tuple(F_DIALOG_ARROW_UP,        Type::GFX,  "dialog02"),
      std::make_tuple(F_DIALOG_ARROW_UP_PRESSED,Type::GFX,  "dialog03"),
      std::make_tuple(F_DIALOG_ARROW_DOWN,      Type::GFX,  "dialog04"),
      std::make_tuple(F_DIALOG_ARROW_DOWN_PRESSED,Type::GFX,"dialog05"),
      std::make_tuple(F_DIALOG_SLIDER_BAR,      Type::GFX,  "dialog06"),
      std::make_tuple(F_DIALOG_SLIDER,          Type::GFX,  "dialog07"),

      // orc directory
      std::make_tuple(F_ORC_00,                 Type::GFX,  "orc00"),
      std::make_tuple(F_ORC_01,                 Type::GFX,  "orc01"),
      std::make_tuple(F_ORC_02,                 Type::GFX,  "orc02"),
      std::make_tuple(F_ORC_03,                 Type::GFX,  "orc03"),
      std::make_tuple(F_ORC_04,                 Type::GFX,  "orc04"),
      std::make_tuple(F_ORC_05,                 Type::GFX,  "orc05"),
      std::make_tuple(F_ORC_06,                 Type::GFX,  "orc06"),
      std::make_tuple(F_ORC_07,                 Type::GFX,  "orc07"),
      std::make_tuple(F_ORC_08,                 Type::GFX,  "orc08"),
      std::make_tuple(F_ORC_09,                 Type::GFX,  "orc09"),
      std::make_tuple(F_ORC_10,                 Type::GFX,  "orc10"),
      std::make_tuple(F_ORC_11,                 Type::GFX,  "orc11"),
      std::make_tuple(F_ORC_12,                 Type::GFX,  "orc12"),
      std::make_tuple(F_ORC_13,                 Type::GFX,  "orc13"),
      std::make_tuple(F_ORC_14,                 Type::GFX,  "orc14"),
      std::make_tuple(F_ORC_15,                 Type::GFX,  "orc15"),
      std::make_tuple(F_ORC_16,                 Type::GFX,  "orc16"),
      std::make_tuple(F_ORC_17,                 Type::GFX,  "orc17"),
      std::make_tuple(F_ORC_18,                 Type::GFX,  "orc18"),
      std::make_tuple(F_ORC_19,                 Type::GFX,  "orc19"),
      std::make_tuple(F_ORC_20,                 Type::GFX,  "orc20"),
      std::make_tuple(F_ORC_21,                 Type::GFX,  "orc21"),
      std::make_tuple(F_ORC_22,                 Type::GFX,  "orc22"),
      std::make_tuple(F_ORC_23,                 Type::GFX,  "orc23"),
      std::make_tuple(F_ORC_24,                 Type::GFX,  "orc24"),
      std::make_tuple(F_ORC_25,                 Type::GFX,  "orc25"),
      std::make_tuple(F_ORC_26,                 Type::GFX,  "orc26"),
      std::make_tuple(F_ORC_27,                 Type::GFX,  "orc27"),
      std::make_tuple(F_ORC_28,                 Type::GFX,  "orc28"),
      std::make_tuple(F_ORC_29,                 Type::GFX,  "orc29"),
      std::make_tuple(F_ORC_30,                 Type::GFX,  "orc30"),
      std::make_tuple(F_ORC_31,                 Type::GFX,  "orc31"),
      std::make_tuple(F_ORC_32,                 Type::GFX,  "orc32"),
      std::make_tuple(F_ORC_33,                 Type::GFX,  "orc33"),
      std::make_tuple(F_ORC_34,                 Type::GFX,  "orc34"),
      std::make_tuple(F_ORC_35,                 Type::GFX,  "orc35"),
      std::make_tuple(F_ORC_36,                 Type::GFX,  "orc36"),
      std::make_tuple(F_ORC_37,                 Type::GFX,  "orc37"),
      std::make_tuple(F_ORC_38,                 Type::GFX,  "orc38"),
      std::make_tuple(F_ORC_39,                 Type::GFX,  "orc39"),
      std::make_tuple(F_ORC_40,                 Type::GFX,  "orc40"),
      std::make_tuple(F_ORC_41,                 Type::GFX,  "orc41"),
      std::make_tuple(F_ORC_42,                 Type::GFX,  "orc42"),
      std::make_tuple(F_ORC_43,                 Type::GFX,  "orc43"),
      std::make_tuple(F_ORC_44,                 Type::GFX,  "orc44"),
      std::make_tuple(F_ORC_45,                 Type::GFX,  "orc45"),
      std::make_tuple(F_ORC_46,                 Type::GFX,  "orc46"),
      std::make_tuple(F_ORC_47,                 Type::GFX,  "orc47"),
      std::make_tuple(F_ORC_48,                 Type::GFX,  "orc48"),
      std::make_tuple(F_ORC_49,                 Type::GFX,  "orc49"),
      std::make_tuple(F_ORC_50,                 Type::GFX,  "orc50"),
      std::make_tuple(F_ORC_51,                 Type::GFX,  "orc51"),
      std::make_tuple(F_ORC_52,                 Type::GFX,  "orc52"),
      std::make_tuple(F_ORC_53,                 Type::GFX,  "orc53"),
      std::make_tuple(F_ORC_54,                 Type::GFX,  "orc54"),
      std::make_tuple(F_ORC_55,                 Type::GFX,  "orc55"),
      std::make_tuple(F_ORC_56,                 Type::GFX,  "orc56"),
      std::make_tuple(F_ORC_57,                 Type::GFX,  "orc57"),
      std::make_tuple(F_ORC_58,                 Type::GFX,  "orc58"),
      std::make_tuple(F_ORC_59,                 Type::GFX,  "orc59"),
      std::make_tuple(F_ORC_60,                 Type::GFX,  "orc60"),
      std::make_tuple(F_ORC_61,                 Type::GFX,  "orc61"),
      std::make_tuple(F_ORC_62,                 Type::GFX,  "orc62"),
      std::make_tuple(F_ORC_63,                 Type::GFX,  "orc63"),
      std::make_tuple(F_ORC_64,                 Type::GFX,  "orc64"),
      std::make_tuple(F_ORC_65,                 Type::GFX,  "orc65"),
      std::make_tuple(F_ORC_66,                 Type::GFX,  "orc66"),
      std::make_tuple(F_ORC_67,                 Type::GFX,  "orc67"),
      std::make_tuple(F_ORC_68,                 Type::GFX,  "orc68"),
      std::make_tuple(F_ORC_69,                 Type::GFX,  "orc69"),
      std::make_tuple(F_ORC_70,                 Type::GFX,  "orc70"),
      std::make_tuple(F_ORC_71,                 Type::GFX,  "orc71"),

      // test directory
      std::make_tuple(F_ALPHA320X200,           Type::GFX,  "alpha320x200"),
      std::make_tuple(F_ALPHA32X32,             Type::GFX,  "alpha32x32"),
      std::make_tuple(F_ICON,                   Type::GFX,  "icon"),
      std::make_tuple(F_INDEX32X32,             Type::GFX,  "index32x32"),
      std::make_tuple(F_INDEX64_48,             Type::GFX,  "index64x48"),
      std::make_tuple(F_INDEX184x112,           Type::GFX,  "index184x112")
   };
   return resourceFiles;
}

Client::TCategoryContent COGLTest::GetCategoryContent()
{
   const Client::TCategoryContent categoryContent = {
      { CAT_STARTUP, { // Splash screen and loading progress
         F_FONT_DUNE3DLARGE, F_FONT_DUNE3DSMALL, F_DIALOG_BACK_SUNKEN, F_DIALOG_BACK_OUTSTANDING,
         F_STARTUP_LOADTEXT, F_STARTUP_PROGRESS_EMPTY, F_STARTUP_PROGRESS_FULL,
         F_STARTUP_MUSIC, F_STARTUP_SOUND, F_STARTUP_DISABLED, F_STARTUP_BACKGROUND,
      } },
      { CAT_COMMON, { // For all categories
         F_FONT_DUNESMALLCAPS, F_FONT_DUNEBLACK, F_FONT_DUNEHOUSE,
         F_DIALOG_ARROW_UP, F_DIALOG_ARROW_UP_PRESSED, F_DIALOG_ARROW_DOWN, F_DIALOG_ARROW_DOWN_PRESSED,
         F_DIALOG_SLIDER_BAR, F_DIALOG_SLIDER,
      } },
      { CAT_TEST, { // OGLTest just needs this one main data category
         F_ALPHA32X32, F_ALPHA320X200, F_ICON, F_INDEX32X32, F_INDEX64_48, F_INDEX184x112
      } },
   };
   return categoryContent;
}
