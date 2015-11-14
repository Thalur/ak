/**
 *
 */
#ifndef AK_RESOURCES_H_INCLUDED
#define AK_RESOURCES_H_INCLUDED

#include <vector>
#include <string>

// Resource categories that can be specified as required by game states
enum EResourceCategory {
   GFXCAT_NONE, GFXCAT_DEFAULT, GFXCAT_INTRO, GFXCAT_MENU
};

enum EFileType {
   FT_TEXT, FT_GFX, FT_SFX, FT_MUSIC, FT_OTHER
};

// All files present in resources to be used directly in the code
enum EResourceFile {
   FILE_FONTWHATEVER,
   FILE_UNIT257,

   NUM_RESOURCE_FILES
};

typedef std::vector<EResourceCategory> TRequiredResources;
typedef std::tuple<EResourceFile, EFileType, std::string> TResourceFile;
typedef std::vector<TResourceFile> TResourceFiles;

bool IsResourceSubset(const TRequiredResources& aSubset, const TRequiredResources& aSuperset);

TResourceFiles GetResourceFiles(const TRequiredResources& aCategories);

#endif // AK_RESOURCES_H_INCLUDED
