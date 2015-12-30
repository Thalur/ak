/**
 *
 */
#ifndef AK_IAPPINTERFACE_H_INCLUDED
#define AK_IAPPINTERFACE_H_INCLUDED

#include "common/cabinet/File.h"
#include <vector>
#include <string>

class IGameState;

namespace Client
{

class IAppInterface;
typedef std::shared_ptr<IAppInterface> TAppPtr;
typedef std::shared_ptr<IGameState> TGameStatePtr;

/**
 * This interface provides the Engine with access to application-specific information.
 * An application subclasses this to create the app functionality.
 * The Engine will call the appropriate methods to run the application.
 */
class IAppInterface
{
public:
   typedef std::vector<std::string> TNames;

   virtual ~IAppInterface() {}


   /// General information

   // Get the application name to be displayed as the window title
   virtual std::string AppName() = 0;


   /// Files and data

   // Get a list of internal files to be loaded immediately on startup
   virtual TNames InternalFiles() = 0;

   // Get a list of resource files, including wirldcard names for patches
   virtual TNames ResourceFiles() = 0;


   /// Running the application

   // Get the current game state
   virtual TGameStatePtr GameState() = 0;

};

} // namespace Client

#endif AK_IAPPINTERFACE_H_INCLUDED
