/**
 *
 */
#include "common/log/log.h"
#include "common/cabinet/CreateCabinet.h"
#include "common/cabinet/PosixFile.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <random>
#include <cstdio>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>


namespace {

using std::cout;
using std::endl;
using TStrings = std::vector<std::string>;
using TCreateCabPtr = std::unique_ptr<CCreateCabinet>;
namespace bfs = boost::filesystem;

const std::string appName { "AK Cabinet Creation tool v0.1 alpha" };

const std::string errorString { "ERROR" };

TStrings excludedFiles;

enum ECommand {
   NONE, SET, KEY, VERSION, PATCH, EXTRACT, EXTRACTALL, EXTRACTAS, EXTRACTTO, EXTRACTTOAS,
   REMOVE, REMOVEALL, RENAME, EXCLUDE, ADD, ADDAS, ADDFROM, ADDFROMAS, SETMODIFY, SETNOMODIFY, VIEW
};

enum EOption {
   EMPTY, NOREPLACE, NOMODIFY, PATH, RECURSIVE
};

enum EArguments {
   ZERO, ONEOPT, ONE, MULTIPLE
};

struct TCommand
{
   std::string iDisplayString;
   TStrings    iCommandStrings;
   std::string iDescription;
   EArguments  iArguments;
   bool        iAllowOptions;
   bool        iModifiesCabinet;
};

const std::map<ECommand, TCommand> commands {
   { NONE, { "NONE", { }, "", ZERO, false, false } },
   { SET, { "SET", { "-s", "-set" }, "-s|-set encrypt|compress|noencrypt|nocompress // Set archive properties", MULTIPLE, false, true } },
   { KEY, { "KEY", { "-k", "-key" }, "-k|-key encryptionKey // Set the encryption key for the archive", ONE, false, false } },
   { VERSION, { "VERSION", { "-version" }, "-version version // Set the cabinet file version for the new cabinet", ONE, false, true } },
   { PATCH, { "PATCH", { "-patch" }, "-patch patchCycle // Set the patch cycle data (v3+ required)", ONE, false, true } },
   { EXTRACTALL, { "EXTRACTALL", { "-extractall" }, "-extractall [folder] // Extract all files to folder", ONEOPT, false, false } },
   { EXTRACT, { "EXTRACT", { "-e", "-extract" }, "-e|-extract{as} file1 {dst1} [file2 {dst2} [...] ] // Store files", MULTIPLE, false, false } },
   { EXTRACTAS, { "EXTRACTAS", { "-eas", "-extractas" }, "", MULTIPLE, false, false } },
   { EXTRACTTO, { "EXTRACTTO", { "-eto", "-extractto" }, "-extractto{as} folder file1 {dst1} [file2 {dst2} [...] ]", MULTIPLE, false, false } },
   { EXTRACTTOAS, { "EXTRACTTOAS", { "-etoas", "-extracttoas" }, "", MULTIPLE, false, false } },
   { REMOVEALL, { "REMOVEALL", { "-c", "-clear", "-removeall" }, "-c|-clear|-removeall // clear the cabinet", ZERO, false, true } },
   { REMOVE, { "REMOVE", { "-r", "-remove" }, "-r|-remove file1 [file2 [...] ] // Remove files from cabinet", MULTIPLE, false, true } },
   { RENAME, { "RENAME", { "-rename" }, "-rename oldName1 newName1 [oldName2 newName2 [...] ] // Rename a file", MULTIPLE, false, true } },
   { EXCLUDE, { "EXCLUDE", { "-exclude" }, "-exclude file1 [file2 [...] ] // exclude files from add folder commands", MULTIPLE, false, false } },
   { ADD, { "ADD", { "-a", "-add" }, "-a|-add file1/dir1 [file2/dir2 [...] ] // Add files or folders", MULTIPLE, true, true } },
   { ADDAS, { "ADDAS", { "-addas" }, "-addas file1 name1 [file2 name2 [...] ] // Add file with new name", MULTIPLE, true, true } },
   { ADDFROM, { "ADDFROM", { "-addfrom" }, "-addfrom{as} cabinet [file1 {dst1} [...] ] // Add from another cabinet", MULTIPLE, true, true } },
   { ADDFROMAS, { "ADDFROMAS", { "-addfromas" }, "", MULTIPLE, true, true } },
   { SETMODIFY, { "SETMODIFY", { "-setmodify" }, "-set{no}modify file1 [file2 [...] ] // (Dis)allow encrypt&compress for file", MULTIPLE, false, true } },
   { SETNOMODIFY, { "SETNOMODIFY", { "-setnomodify" }, "", MULTIPLE, false, true } },
   { VIEW, { "VIEW", { "-v", "-view" }, "-v|-view // View cabinet content", ZERO, false, false } }
};

inline const std::string& ToString(ECommand aCommand)
{
   auto it = commands.find(aCommand);
   return it != commands.end() ? it->second.iDisplayString : errorString;
}

struct TOption
{
   std::string iDisplayString;
   TStrings    iOptionStrings;
   std::string iDescription;
};

const std::map<EOption, TOption> options {
   { EMPTY, { "EMPTY", { }, "" } },
   { NOREPLACE, { "NOREPLACE", { "-noreplace" }, "-noreplace // do not replace an existing file within the cabinet" } },
   { NOMODIFY, { "NOMODIFY", { "-nomodify" }, "-nomodify // do not compress or encrypt the file(s)" } },
   { PATH, { "PATH", { "-path" }, "-path // add the relative path to the file name(s)" } },
   { RECURSIVE, { "RECURSIVE", { "-recursive" }, "-recursive // include subdirectories when specifying a directory" } }
};

inline const std::string& ToString(EOption aOption)
{
   auto it = options.find(aOption);
   return it != options.end() ? it->second.iDisplayString : errorString;
}

struct TParameter
{
   ECommand iCommand;
   std::vector<EOption> iOptions;
   TStrings iData;

   TParameter(ECommand aCommand, std::vector<EOption>&& aOptions, TStrings&& aData)
    : iCommand(aCommand), iOptions(aOptions), iData(aData)
   {}

   bool CanAddOpt() const
   {
      auto cmd = commands.find(iCommand);
      return (cmd != commands.end()) && cmd->second.iAllowOptions;
   }
   bool CanAddArg() const
   {
      auto cmd = commands.find(iCommand);
      return (cmd != commands.end()) && ((cmd->second.iArguments == MULTIPLE) ||
             ((cmd->second.iArguments == ONE || cmd->second.iArguments == ONEOPT) && iData.empty()));
   }
   bool HasSufficientArgs() const
   {
      auto cmd = commands.find(iCommand);
      return (cmd != commands.end()) && (!iData.empty() || (cmd->second.iArguments == ZERO) || (cmd->second.iArguments == ONEOPT));
   }
   bool ModifiesCabinet() const
   {
      auto cmd = commands.find(iCommand);
      return (cmd != commands.end()) && (cmd->second.iModifiesCabinet);
   }
   bool HasOption(EOption aOption) const
   {
      auto opt = std::find(iOptions.begin(), iOptions.end(), aOption);
      return (opt != iOptions.end());
   }

   bool operator < (const TParameter& cmp) const
   {
      return iCommand < cmp.iCommand;
   }
   bool operator == (const TParameter& cmp) const
   {
      return iCommand == cmp.iCommand;
   }
};

std::vector<TParameter>::const_iterator GetParam(const std::vector<TParameter>& aParams, ECommand aCommand)
{
   return std::find(aParams.cbegin(), aParams.cend(), TParameter(aCommand, std::vector<EOption>(), TStrings()));
}

ECommand GetCommandType(const std::string& aName)
{
   for (const auto& cmd : commands) {
      for (const std::string& s : cmd.second.iCommandStrings) {
         if (aName == s) {
            return cmd.first;
         }
      }
   }
   return NONE;
}

EOption GetOptionType(const std::string& aName)
{
   for (const auto& opt : options) {
      for (const std::string& s : opt.second.iOptionStrings) {
         if (aName == s) {
            return opt.first;
         }
      }
   }
   return EMPTY;
}

void PrintUsage()
{
	cout << "USAGE:" << endl;
	cout << "> AKCab.exe CABINET [COMMAND [OPTIONS] PARAMS]+" << endl;
	cout << " * CABINET: (new or existing) cabinet filename" << endl;
	cout << " * COMAMNDS: (executed in this order)" << endl;
   for (const auto& cmd : commands) {
      if ((cmd.first != NONE) && (cmd.second.iDescription.length() > 0)) {
         cout << "    " << cmd.second.iDescription << endl;
      }
   }
   cout << " * OPTIONS for ADD commands: (valid for the respective command)" << endl;
   for (const auto& option : options) {
      if ((option.first != EMPTY) && (option.second.iDescription.length() > 0)) {
         cout << "    " << option.second.iDescription << endl;
      }
   }
}

void AddCommand(std::vector<TParameter>& aParams, TParameter& aCurrent)
{
   if (aCurrent.iCommand != NONE) {
      if (!aCurrent.HasSufficientArgs()) {
         cout << "Ignoring incomplete command " << ToString(aCurrent.iCommand) << endl;
      } else {
         aParams.emplace_back(aCurrent.iCommand, std::move(aCurrent.iOptions), std::move(aCurrent.iData));
      }
      aCurrent.iOptions.clear();
      aCurrent.iData.clear();
   }
}

std::vector<TParameter> ParseArguments(int argc, char** argv)
{
   std::vector<TParameter> params;
   TParameter current { NONE, { }, { } };
   for (int i = 2; i < argc; i++) {
      std::string arg = argv[i];
      ECommand cmd = GetCommandType(arg);
      if (cmd != NONE) { // Start new command
         AddCommand(params, current);
         current.iCommand = cmd;
      } else if (current.iCommand != NONE) { // Enhance current command
         EOption opt = GetOptionType(arg);
         if (opt != EMPTY) {
            if (current.CanAddOpt()) {
               current.iOptions.emplace_back(opt);
            } else {
               cout << "Ignoring invalid option " << opt << " for command " << ToString(current.iCommand) << endl;
            }
         } else if (current.CanAddArg()) {
            current.iData.emplace_back(std::move(arg));
         } else {
            cout << "Ignoring invalid parameter " << arg << " for command " << ToString(current.iCommand) << endl;
         }
      } else {
         cout << "Ignoring invalid command " << arg << endl;
      }
   }
   AddCommand(params, current);
   std::sort(params.begin(), params.end());
   return params;
}

/*std::pair<CCreateCabinet, bool> PrepareCabinet(const std::string& aFilename, const std::vector<TParameter>& aParams)
{
   TPosixFilePtr dest = CPosixFile::CreateEmptyFile(aFilename+".temp");
   if (!dest) {
      cout << "Error creating temporary file " << aFilename << ".temp" << endl;
      return std::make_pair(CCreateCabinet(), false);
   }
   auto it = GetParam(aParams, KEY);
   if (it != aParams.cend()) {
      try {
         key = std::stoi(it->iData[0]);
      } catch (const std::exception&) {
         cout << "Invalid KEY parameter: " << it->iData[0] << endl;
      }
   }
   TFilePtr cabFile = CPosixFile::OpenExistingFile(aFilename, false);
   if (!cabFile) {
      CCreateCabinet create;
      create.SetDestFile(dest);
      create.SetEncryptionKey(key);
      return std::make_pair(create, true);
   }
   TCabinetPtr cab = CCabinet::Open(cabFile, key);
   if (!cab) {
      cout << "Error reading cabinet file " << aFilename << endl;
      return std::make_pair(CCreateCabinet(), false);
   }
   CCreateCabinet create = CCreateCabinet(cab);
   create.SetDestFile(dest);
   return std::make_pair(create, true);
}*/

int ExtractFile(CCabinet& aCabinet, const std::string& aCabFile, const std::string& aDestDir, const std::string& aDestFile)
{
   std::string destFileName { aDestFile };
   if (aDestDir.length() > 0) {
      destFileName = aDestDir + '/' + aDestFile;
   }
   TMemoryFilePtr filePtr = aCabinet.ReadFileByName(aCabFile);
   if (filePtr) {
      TPosixFilePtr destFile = CPosixFile::CreateEmptyFile(destFileName);
      if (destFile) {
         if (destFile->Overwrite(filePtr->GetBuffer(), 0)) {
            return 1;
         }
      }
   }
   cout << "Error extracting file " << aCabFile << " to " << destFileName << endl;
   return 0;
}

int ExecuteRead(CCabinet& aCabinet, const TParameter& aParam)
{
   int numExtracted = 0;
   TSize size = aParam.iData.size();
   std::string dir;

   switch (aParam.iCommand) {
   case EXTRACT:
      for (auto s : aParam.iData) {
         numExtracted += ExtractFile(aCabinet, s, dir, s);
      }
      break;
   case EXTRACTALL:
   {
      if (size > 0) {
         dir = aParam.iData[0];
      }
      auto files = aCabinet.GetFileList();
      for (auto file : files) {
         numExtracted += ExtractFile(aCabinet, file.first, dir, file.first);
      }
      break;
   }
   case EXTRACTAS:
      for (TSize i = 1; i < size; i += 2) {
         numExtracted += ExtractFile(aCabinet, aParam.iData[i-1], dir, aParam.iData[i]);
      }
      if (size%2 != 0) {
         cout << "Ignoring dangling parameter to EXTRACTAS: " << aParam.iData[size-1] << endl;
      }
      break;
   case EXTRACTTO:
      dir = aParam.iData[0];
      for (TSize i = 1; i < size; i++) {
         numExtracted += ExtractFile(aCabinet, aParam.iData[i], dir, aParam.iData[i]);
      }
      break;
   case EXTRACTTOAS:
      dir = aParam.iData[0];
      for (TSize i = 2; i < size; i += 2) {
         numExtracted += ExtractFile(aCabinet, aParam.iData[i-1], dir, aParam.iData[i]);
      }
      if (size%2 == 0) {
         cout << "Ignoring dangling parameter to EXTRACTTOAS: " << aParam.iData[size-1] << endl;
      }
      break;
   case EXCLUDE:
      for (auto s : aParam.iData) {
         excludedFiles.emplace_back(s);
      }
      break;
   default:
      // necessary for clang/osx warning
      break;
   }
   return numExtracted;
}

bfs::path RelativeTo(bfs::path from, bfs::path to)
{
   // Start at the root path and while they are the same then do nothing then when they first
   // diverge take the remainder of the two path and replace the entire from path with ".."
   // segments.
   bfs::path::const_iterator fromIter = from.begin();
   bfs::path::const_iterator toIter = to.begin();

   // Loop through both
   while (fromIter != from.end() && toIter != to.end() && (*toIter) == (*fromIter))
   {
      ++toIter;
      ++fromIter;
   }

   bfs::path finalPath;
   while (fromIter != from.end())
   {
      finalPath /= "..";
      ++fromIter;
   }

   while (toIter != to.end())
   {
      finalPath /= *toIter;
      ++toIter;
   }
   return finalPath;
}


bool AddFile(CCreateCabinet& aCabinet, const bfs::path& aFile, const bfs::path& aDestFile,
             bool aOverwrite, bool aEncryptCompress, bool aIncludePath)
{
   std::string destFileName;
   if (aIncludePath) {
      static const bfs::path currentPath(bfs::initial_path());
      destFileName = RelativeTo(currentPath, aFile).generic_string();
   } else {
      destFileName = aDestFile.generic_string();
   }
   cout << "Adding file " << aFile.generic_string() << " as " << destFileName << endl;
   TFilePtr file = CPosixFile::OpenExistingFile(aFile.generic_string(), false);
   if (file) {
      if (aOverwrite) {
         aCabinet.RemoveFile(destFileName);
      }
      return aCabinet.AddFile(destFileName, file, aEncryptCompress, aEncryptCompress);
   }
   return false;
}

bool AddFromCabinet(CCreateCabinet& aCabinet, TCabinetPtr aSrcCabinet, const std::string& aSourceFilename,
                    const std::string& aDestFilename, bool aOverwrite, bool aEncryptCompress)
{
   cout << "Adding cabinet file " << aSourceFilename << " as " << aDestFilename << endl;
   TSize index;
   if (aSrcCabinet->GetFileIndex(index, aSourceFilename)) {
      if (aOverwrite) {
         aCabinet.RemoveFile(aDestFilename);
      }
      return aCabinet.AddFile(aDestFilename, aSrcCabinet, index, aEncryptCompress, aEncryptCompress);
   }
   return false;
}

bool ExecuteWrite(CCreateCabinet& aCabinet, const TParameter& aParam)
{
   bool bChanged = false;
   TSize size = aParam.iData.size();
   bool bOverwrite = !aParam.HasOption(NOREPLACE);
   bool bModify = !aParam.HasOption(NOMODIFY);
   bool bIncludePath = aParam.HasOption(PATH);
   bool bRecursive = aParam.HasOption(RECURSIVE);

   switch (aParam.iCommand) {
   case SET:
      for (auto s : aParam.iData) {
         if (s == "encrypt") {
            if (aCabinet.GetEncryption() != CCabinet::ENCRYPT_AKV2) {
               aCabinet.SetEncryption(CCabinet::ENCRYPT_AKV2);
               bChanged = true;
            }
         } else if (s == "compress") {
            if (aCabinet.GetCompression() != CCabinet::COMPRESS_ZIP) {
               aCabinet.SetCompression(CCabinet::COMPRESS_ZIP);
               bChanged = true;
            }
         } else if (s == "noencrypt") {
            if (aCabinet.GetEncryption() != CCabinet::ENCRYPT_NONE) {
               aCabinet.SetEncryption(CCabinet::ENCRYPT_NONE);
               bChanged = true;
            }
         } else if (s == "nocompress") {
            if (aCabinet.GetCompression() != CCabinet::COMPRESS_NONE) {
               aCabinet.SetCompression(CCabinet::COMPRESS_NONE);
               bChanged = true;
            }
         } else {
            cout << "Ignoring invalid parameter for " << ToString(SET) << ": " << s << endl;
         }
      }
      break;
   case VERSION:
      try {
         int version = std::stoi(aParam.iData[0]);
         if (aCabinet.GetCabinetVersion() != version) {
            aCabinet.SetCabinetVersion(version);
            bChanged = true;
         }
      } catch (const std::exception&) {
         cout << "Invalid VERSION parameter: " << aParam.iData[0] << endl;
      }
      break;
   case PATCH:
      try {
         int patch = std::stoi(aParam.iData[0]);
         if (aCabinet.GetPatchCycle() != patch) {
            aCabinet.SetPatchCycle(patch);
            bChanged = true;
         }
      } catch (const std::exception&) {
         cout << "Invalid PATCH parameter: " << aParam.iData[0] << endl;
      }
      break;
   case REMOVE:
      for (auto s : aParam.iData) {
         bChanged |= aCabinet.RemoveFile(s);
      }
      break;
   case REMOVEALL:
      bChanged = aCabinet.GetNumFiles() > 0;
      aCabinet.Clear();
      break;
   case RENAME:
      for (TSize i = 1; i < size; i += 2) {
         bChanged = aCabinet.RenameFile(aParam.iData[i-1], aParam.iData[i]);
      }
      if (size%2 != 0) {
         cout << "Ignoring dangling parameter to RENAME: " << aParam.iData[size-1] << endl;
      }
      break;
   case ADD:
      for (auto s : aParam.iData) {
         try {
            bfs::path p(s);
            if (!bfs::exists(p)) {
               cout << "ADD location not found: " << s << endl;
            } else {
               if (bfs::is_regular_file(p)) {
                  bChanged |= AddFile(aCabinet, bfs::canonical(p), p.filename(), bOverwrite, bModify, bIncludePath);
               } else if (bfs::is_directory(p)) {
                  if (bRecursive) {
                     for (bfs::directory_entry& x : bfs::recursive_directory_iterator(p)) {
                        if (bfs::is_regular_file(x.path())) {
                           bChanged |= AddFile(aCabinet, bfs::canonical(x.path()), x.path().filename(), bOverwrite, bModify, bIncludePath);
                        }
                     }
                  } else {
                     for (bfs::directory_entry& x : bfs::directory_iterator(p)) {
                        if (bfs::is_regular_file(x.path())) {
                           bChanged |= AddFile(aCabinet, bfs::canonical(x.path()), x.path().filename(), bOverwrite, bModify, bIncludePath);
                        }
                     }
                  }
               } else {
                  cout << "Error processing location " << s << endl;
               }
            }
         } catch (const bfs::filesystem_error& ex)
         {
            cout << "Error accessing location " << s << ": " << ex.what() << endl;
         }
      }
      break;
   case ADDAS:
      for (TSize i = 1; i < size; i += 2) {
         const std::string& s(aParam.iData[i-1]);
         try {
            bfs::path p(s);
            if (!bfs::exists(p) || !bfs::is_regular_file(p)) {
               cout << "File not found: " << s << endl;
            } else {
               bChanged |= AddFile(aCabinet, bfs::canonical(p), bfs::path(aParam.iData[i]), bOverwrite, bModify, false);
            }
         } catch (const bfs::filesystem_error& ex)
         {
            cout << "Error accessing location " << s << ": " << ex.what() << endl;
         }
      }
      if (size%2 != 0) {
         cout << "Ignoring dangling parameter to ADDAS: " << aParam.iData[size-1] << endl;
      }
      break;
   case ADDFROM:
   {
      TFilePtr cabFile = CPosixFile::OpenExistingFile(aParam.iData[0], false);
      if (cabFile) {
         TCabinetPtr sourceCab = CCabinet::Open(cabFile, aCabinet.GetEncryptionKey());
         if (sourceCab) {
            std::vector<std::string> fileList;
            if (size <= 1) { // Add all files from the cabinet
               auto entries = sourceCab->GetFileList();
               for (auto& entry : entries) {
                  fileList.push_back(entry.first);
               }
            } else {
               for (TSize i = 1; i < size; i++) {
                  fileList.push_back(aParam.iData[i]);
               }
            }
            for (auto s : fileList) {
               bChanged |= AddFromCabinet(aCabinet, sourceCab, s, s, bOverwrite, bModify);
            }
         } else {
            cout << "Could not read cabinet " << aParam.iData[0] << endl;
         }
      } else {
        cout << "Could not open file " << aParam.iData[0] << endl;
      }
      break;
   }
   case ADDFROMAS:
   {
      TFilePtr cabFile = CPosixFile::OpenExistingFile(aParam.iData[0], false);
      if (cabFile) {
         TCabinetPtr sourceCab = CCabinet::Open(cabFile, aCabinet.GetEncryptionKey());
         if (sourceCab) {
            for (TSize i = 2; i < size; i += 2) {
               bChanged |= AddFromCabinet(aCabinet, sourceCab, aParam.iData[i-1], aParam.iData[i], bOverwrite, bModify);
            }
            if (size%2 == 0) {
               cout << "Ignoring dangling parameter to ADDFROMAS: " << aParam.iData[size-1] << endl;
            }
         } else {
            cout << "Could not read cabinet " << aParam.iData[0] << endl;
         }
      } else {
         cout << "Could not open file " << aParam.iData[0] << endl;
      }
      break;
   }
   case SETMODIFY:
   case SETNOMODIFY:
   {
      bool modify = (aParam.iCommand == SETMODIFY);
      for (auto s : aParam.iData) {
         bChanged |= aCabinet.SetFileProperties(s, modify, modify);
      }
      break;
   }
   default:
      // necessary for clang/osx warning
      break;
   }
   return bChanged;
}

bool ExecuteReadCommands(CCabinet& aCabinet, const std::vector<TParameter>& aParams)
{
   bool bHasWriteCommand = false;
   int numExtracted = 0;
   for (auto param : aParams) {
      if (param.ModifiesCabinet()) {
         bHasWriteCommand = true;
      } else {
         numExtracted += ExecuteRead(aCabinet, param);
      }
   }
   if (numExtracted > 0) {
      cout << "Extracted " << numExtracted << " files from the cabinet." << endl;
   }
   return bHasWriteCommand;
}

bool ExecuteWriteCommands(CCreateCabinet& aCabinet, const std::vector<TParameter>& aParams)
{
   bool bCabinetModified = false;
   for (auto param : aParams) {
      if (param.ModifiesCabinet()) {
         bCabinetModified |= ExecuteWrite(aCabinet, param);
      }
   }
   return bCabinetModified;
}

uint32_t GetRandomNumber()
{
   static std::mt19937 rng{std::random_device()()};
   static std::uniform_int_distribution<uint32_t> dist;
   return dist(rng);
}

} // anonymous namespace


int main(int argc, char** argv)
{
   NLogging::InitLogFile(appName, "log.txt", ELogLevel::EDEBUG, ELogLevel::EINFO);
   cout << appName << endl;
   std::string cabFileName;
   if (argc >= 2) cabFileName = argv[1];
   if (cabFileName.empty() || (NONE != GetCommandType(cabFileName)) || (EMPTY != GetOptionType(cabFileName))) {
      PrintUsage();
   } else {
      // Parse command line arguments
      std::vector<TParameter> params = ParseArguments(argc, argv);
      int32_t key = 0;
      auto it = GetParam(params, KEY);
      if (it != params.cend()) {
         try {
            key = std::stoi(it->iData[0]);
         } catch (const std::exception&) {
            cout << "Invalid KEY parameter: " << it->iData[0] << endl;
         }
      }
      bool bView = params.empty() || (GetParam(params, VIEW) != params.cend()) ||
                   ((params.size() == 1) && (GetParam(params, KEY) != params.cend()));

      // Open the given cabinet and execute read commands
      TCabinetPtr sourceCab;
      TFilePtr cabFile = CPosixFile::OpenExistingFile(cabFileName, false);
      if (cabFile) {
         sourceCab = CCabinet::Open(cabFile, key);
      }
      bool hasWriteCommands = ExecuteReadCommands(*sourceCab, params);

      // Create temporary cabinet and execute write commands
      if (hasWriteCommands) {
         uint32_t rnd = GetRandomNumber();
         std::string newCabFileName = sourceCab ? cabFileName + "." + std::to_string(rnd) + ".temp" : cabFileName;
         TPosixFilePtr destFile = CPosixFile::CreateEmptyFile(newCabFileName);
         if (!destFile) {
            cout << "Error creating temporary file " << newCabFileName << endl;
         } else {
            TCreateCabPtr destCab;
            if (sourceCab) {
               destCab = ::make_unique<CCreateCabinet>(sourceCab);
            } else {
               destCab = ::make_unique<CCreateCabinet>();
            }
            destCab->SetDestFile(destFile);
            destCab->SetEncryptionKey(key);
            bool bMoveRequired = ExecuteWriteCommands(*destCab, params);

            // Execute view, move new cabinet to old file if necessary, delete temp file
            if (bView) {
               destCab->PrintFileIndex();
            }
            if (bMoveRequired) {
               destCab->WriteToDisk();
            }
            destFile->Close();
            if (cabFile) {
               cabFile->Close();
            }
            if (bMoveRequired && cabFile) { // only move if we did not write to the correct file
               if (int result = std::remove(cabFileName.c_str())) {
                  cout << "Could not remove original cabinet file " << cabFileName << " (" << result << ")" << endl;
               } else if (int renameResult = std::rename(newCabFileName.c_str(), cabFileName.c_str())) {
                  cout << "Could not rename temporary cabinet file to " << cabFileName << " (" << renameResult << ")" << endl;
               }
            } else if (!bMoveRequired) {
               if (int result = std::remove(newCabFileName.c_str())) {
                  cout << "Could not remove temporary file " << cabFileName << ".temp (" << result << ")" << endl;
               }
            }
         }
      } else if (bView) {
         if (sourceCab) {
            sourceCab->PrintFileIndex();
         } else {
            cout << "Error opening cabinet " << cabFileName << endl;
         }
      }
   }
   return 0;
}
