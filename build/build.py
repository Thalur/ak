#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
# ToDo list:
#  * add cmake parameters to allow generation (no GUI required)
#  * add missing build target systems: linux, android, osx, ios
#  * generalize build configurations by having one config file per target
#
import sys
import os
import glob
import shutil
from subprocess import call

# For now, all build targets and options are hard-coded in this file
glob_targets = [ [ "akcab", "    - Cabinet file command line tool" ],
                 [ "animview", " - Animation file viewer" ] ]
glob_systems = [ [ "win", "     - Windows 64 bit (windowed or console, depending on target)" ],
                 [ "linux", "   - Linux 64 bit (host operating system binaries)" ],
                 [ "osx", "     - OSX 64 bit" ],
                 [ "android", " - Android NDK package (requires API level 9)" ],
                 [ "ios", "     - iOS package (unspecified)" ] ]
glob_modes = [ [ "debug", "   - Debug build with full log output" ],
               [ "release", " - Release build without debug logs" ] ]
glob_target_supports_system = [
  # win,   linux, osx,  android, ios
  [ True,  True,  True,  False, False ], # akcab
  [ True,  True,  True,  True,  True  ] ] # animview
glob_target_requires_client = [
  #akcab, animview
   False, True ]


def print_usage():
  print "USAGE: python build.py [<TARGET>+] [<SYSTEM>] [<MODE>] [CLEAN]"
  print "TARGET: (default: ALL)"
  for target in glob_targets:
    print " * " + target[0] + target[1]
  print "SYSTEM: (default: host operating system)"
  for system in glob_systems:
    print " * " + system[0] + system[1]
  print "MODE: (default: DEBUG)"
  for mode in glob_modes:
    print " * " + mode[0] + mode[1]
  print "CLEAN: first clean the workspace to re-compile all sources"

def option_contains(options, selection):
  i = 0
  for option in options:
    if selection == option[0]:
      return i
    i += 1
  return -1

def check_config(target, system, mode):
  target_index = option_contains(glob_targets, target)
  system_index = option_contains(glob_systems, system)
  return target_index >= 0 and system_index >= 0 and option_contains(glob_modes, mode) >= 0 and \
         glob_target_supports_system[target_index][system_index]

class cd:
  def __init__(self, newPath):
    self.newPath = os.path.expanduser(newPath)

  def __enter__(self):
    self.savedPath = os.getcwd()
    os.chdir(self.newPath)

  def __exit__(self, etype, value, traceback):
    os.chdir(self.savedPath)

#####
# Build configuration implementations (per target system)
#####

def build_win(target, mode):
  try:
    config = "-D_AK_PROJECT=" + target.upper()
    client = "-D_AK_CLIENT=0"
    target_index = option_contains(glob_targets, target)
    if glob_target_requires_client[target_index]:
      client = "-D_AK_CLIENT=1"
    cmakeCmd = [ 'cmake', '-G', 'Visual Studio 12 2013 Win64', config, client, '-D_AK_TARGET=WIN64', '../../../../src']
    res = call(cmakeCmd, shell=True)
    if res != 0:
      return False
    buildCmd = "cmake --build . --config " + mode.upper()
    res = call(buildCmd, shell=True)
    return res == 0
  except KeyboardInterrupt: # do not crash on Ctrl+C
    return False

def build_linux(target, mode):
  try:
    config = "-D_AK_PROJECT=" + target.upper()
    client = "-D_AK_CLIENT=0"
    target_index = option_contains(glob_targets, target)
    if glob_target_requires_client[target_index]:
      client = "-D_AK_CLIENT=1"
    cmakeCmd = [ 'cmake', '-G', 'Unix Makefiles', config, client, '-D_AK_TARGET=LINUX', "-DCMAKE_BUILD_TYPE="+mode, '../../../../src']
    res = call(cmakeCmd)
    if res != 0:
      return False
    res = call('make')
    return res == 0
  except KeyboardInterrupt: # do not crash on Ctrl+C
    return False

#####

def build_nyi(target, mode, clean):
  print "This configuration has not been implemented yet."
  return False

def copy_files(source, target):
  for file in glob.glob(source):
    shutil.copy2(file, target)

def run_build(target, system, mode, clean):
  print "\n*** Starting build of " + target + " for " + system + " (" + mode + ")..."
  buildResult = False;
  outputDir = os.path.join("..", "bin", "cmake", target, system)
  if clean and os.path.exists(outputDir):
    print "*** Cleaning workspace by removing " + outputDir
    shutil.rmtree(outputDir)
  if not os.path.exists(outputDir):
    os.makedirs(outputDir)
  with cd(outputDir):
    build_fct = "build_" + system
    buildResult = globals().get(build_fct, build_nyi)(target, mode)
  if buildResult:
    binDir = os.path.join("..", "bin", target, system, mode)
    if not os.path.exists(binDir):
      os.makedirs(binDir)
    # TODO: generalize for all projects / targets
    if system == "win":
      copy_files(outputDir+"/projects/"+target+"/"+mode+"/*.exe", binDir)
      copy_files(outputDir+"/projects/"+target+"/"+mode+"/*.pdb", binDir)
      if (target == "animview"):
        shutil.copy2("../lib/freeglut/bin/x64/freeglut.dll", binDir)
        shutil.copy2("../lib/glew-1.12.0/bin/Release/x64/glew32.dll", binDir)
    elif system == "linux":
      shutil.copy2(outputDir+"/projects/"+target+"/AKCab", binDir)
  return buildResult

def get_targets(argv, target_system):
  targets = []
  if argv:
    for arg in argv:
      if option_contains(glob_targets, arg.lower()) >= 0:
        targets.append(arg.lower())
      elif arg.lower() == "all":
        print "Running ALL build configurations for " + target_system
        targets = []
        system_index = option_contains(glob_systems, target_system)
        i = 0
        for target in glob_targets:
          if glob_target_supports_system[i][system_index]:
            targets.append(target[0])
        return targets
      else:
        print "Unknown target specified: " + arg
  return targets

def get_clean(argv):
  clean = False
  for arg in argv:
    if arg.lower() == "clean":
      clean = True
      argv.remove(arg)
  return clean

def get_mode(argv):
  mode = "debug"
  for arg in argv:
    if option_contains(glob_modes, arg.lower()) >= 0:
      mode = arg.lower()
      argv.remove(arg)
  return mode

def get_system(argv):
  os = ""
  for arg in argv:
    if option_contains(glob_systems, arg.lower()) >= 0:
      os = arg.lower()
      argv.remove(arg)
  if not os:
    platform = sys.platform
    if platform == "win32":
      os = "win"
    elif platform == "linux" or platform == "linux2":
      os = "linux"
    elif platform == "darwin":
      os = "osx"
    if os:
      print "Auto-detected platform " + os + " from " + platform
    else:
      print "Error: could not detect host platform from " + platform
  return os

def main(argv):
  if not os.path.isfile("build.py"):
    print "ERROR: This script needs to be executed from the build/ directory."
    return -1
  system = get_system(argv)
  mode = get_mode(argv)
  clean = get_clean(argv)
  targets = get_targets(argv, system)
  if system and targets and mode:
    for target in targets:
      if check_config(target, system, mode):
        if not run_build(target, system, mode, clean):
          print "*** ERROR - BUILD STOPPED"
          return -1
      else:
        print "*** Skipping unsupported configuration: " + target + " " + system + " " + mode
    print "*** BUILD COMPLETE"
  else:
    print_usage()
  return 0

if __name__ == "__main__":
  dir = os.path.dirname(os.path.realpath(sys.argv[0]))
  with cd(dir):
    exit(main(sys.argv[1:]))

