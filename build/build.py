#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
# ToDo list:
#  * add missing build target systems: osx, ios
#
import sys
import os
import glob
import shutil
from subprocess import call

from colored import *
import prj_akcab
import prj_ogltest

# For now, all build targets and options are hard-coded in this file
glob_targets = [ [ "akcab", "    - Cabinet file command line tool" ],
                 [ "ogltest", "  - OpenGL engine test project" ],
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
  [ True,  True,  True,  True,  True  ], # ogltest
  [ True,  True,  True,  True,  True  ] ] # animview


def print_usage():
  print cyan(bright("USAGE: python build.py [<TARGET>+] [<SYSTEM>] [<MODE>] [CLEAN] [INSTALL]"))
  print "TARGET: (default: ALL)"
  for target in glob_targets:
    print " * " + yellow(bright(target[0])) + target[1]
  print "SYSTEM: (default: host operating system)"
  for system in glob_systems:
    print " * " + yellow(bright(system[0])) + system[1]
  print "MODE: (default: DEBUG)"
  for mode in glob_modes:
    print " * " + yellow(bright(mode[0])) + mode[1]
  print "CLEAN: first clean the workspace to re-compile all sources"
  print "INSTALL: after successful build, install app on device (Android/iOS only)"

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

def get_project(target, system, mode, install):
  target_index = option_contains(glob_targets, target)
  system_index = option_contains(glob_systems, system)
  prj = None
  if target_index >= 0 and system_index >= 0 and option_contains(glob_modes, mode) >= 0:
    if target == "akcab":
      prj = prj_akcab.project_akcab(target, system, mode, install)
    elif target == "ogltest":
      prj = prj_ogltest.project_ogltest(target, system, mode, install)
    elif target == "animview":
      prj = prj_animview.project_animview(target, system, mode, install)
  if prj != None and not system in prj.get_supported_systems():
    prj = None # unsupported configuration
  return prj

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

def build_win(prj):
  cmakeCmd = [ 'cmake', '-G', 'Visual Studio 12 2013 Win64', prj.config, prj.client, '-D_AK_TARGET=WIN64', '../../../../src']
  if call(cmakeCmd, shell=True) != 0:
    return False
  buildCmd = "cmake --build . --config " + prj.mode.upper()
  return call(buildCmd, shell=True) == 0

def build_linux(prj):
  cmakeCmd = [ 'cmake', '-G', 'Unix Makefiles', prj.config, prj.client, '-D_AK_TARGET=LINUX', "-DCMAKE_BUILD_TYPE="+prj.mode, '../../../../src']
  if call(cmakeCmd) != 0:
    return False
  return call('make') == 0

def build_android(prj):
  # http://spin.atomicobject.com/2011/08/22/building-android-application-bundles-apks-by-hand/
  ndk = os.environ['ANDROID_NDK']
  print "Detected NDK path as " + ndk

  # Generate the cmake-android toolchain
  cmakeCmd = [ 'cmake', '-DCMAKE_TOOLCHAIN_FILE=../../../../lib/android-cmake/android.toolchain.cmake',
               '-DANDROID_ABI=armeabi', '-DANDROID_NATIVE_API_LEVEL=android-9',
               '-DANDROID_NDK='+ndk, '-DCMAKE_VERBOSE_MAKEFILE=OFF',
               prj.config, prj.client, '-D_AK_TARGET=ANDROID', "-DCMAKE_BUILD_TYPE="+prj.mode, '../../../../src']
  if call(cmakeCmd) != 0:
    return False

  # Run the build with cmake
  buildCmd = "cmake --build . --config " + prj.mode.upper()
  return call(buildCmd, shell=True) == 0

def build_osx(prj):
  cmakeCmd = [ 'cmake', '-G', 'Unix Makefiles', prj.config, prj.client, '-D_AK_TARGET=OSX', "-DCMAKE_BUILD_TYPE="+prj.mode, '../../../../src']
  if call(cmakeCmd) != 0:
    return False
  return call('make') == 0

def build_nyi(prj):
  print magenta(bright("This configuration has not been implemented yet."))
  return False

def copy_files(source, target):
  for file in glob.glob(source):
    shutil.copy2(file, target)

def run_build(prj, clean):
  try:
    print cyan(bright("\n*** Starting build of " + prj.target + " for " + prj.system + " (" + prj.mode + ")..."))
    buildResult = False;
    outputDir = os.path.join("..", "gen", "cmake", prj.target, prj.system)
    if clean and os.path.exists(outputDir):
      print "*** Cleaning workspace by removing " + outputDir
      shutil.rmtree(outputDir)
    if not os.path.exists(outputDir):
      os.makedirs(outputDir)
    with cd(outputDir):
      if prj.pre_build_actions():
        build_fct = "build_" + prj.system
        buildResult = globals().get(build_fct, build_nyi)(prj)
      if buildResult:
        binDir = os.path.join("..", "..", "..", prj.target, prj.system, prj.mode)
        if not os.path.exists(binDir):
          os.makedirs(binDir)
        post_action = "post_build_actions_" + prj.system
        method = getattr(prj, post_action, build_nyi)
        if method:
          buildResult = method(binDir)
        else:
          buildResult = False
    return buildResult
  except KeyboardInterrupt: # do not crash on Ctrl+C
    print magenta(bright("*** Keyboard interrupt detected"))
    return False

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
        print magenta(bright("Unknown target specified: " + arg))
  return targets

def get_clean(argv):
  clean = False
  for arg in argv:
    if arg.lower() == "clean":
      clean = True
      argv.remove(arg)
  return clean

def get_install(argv):
  install = False
  for arg in argv:
    if arg.lower() == "install":
      install = True
      argv.remove(arg)
  return install

def get_mode(argv):
  mode = "debug"
  for arg in argv:
    if option_contains(glob_modes, arg.lower()) >= 0:
      mode = arg.lower()
      argv.remove(arg)
  return mode

def get_system(argv, host_os):
  os = ""
  for arg in argv:
    if option_contains(glob_systems, arg.lower()) >= 0:
      os = arg.lower()
      argv.remove(arg)
  if not os:
    os = host_os
  return os

def get_host_os():
  os = ""
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
    print magenta(bright("Error: could not detect host platform from " + platform))
  return os

def main(argv):
  host_os = get_host_os()
  init_color(host_os)
  if not os.path.isfile("build.py"):
    print magenta(bright("ERROR: This script needs to be executed from the build/ directory."))
    return -1
  clean = get_clean(argv)
  install = get_install(argv)
  system = get_system(argv, host_os)
  mode = get_mode(argv)
  targets = get_targets(argv, system)
  if system and targets and mode:
    for target in targets:
      prj = get_project(target, system, mode, install)
      if prj != None:
        if not run_build(prj, clean):
          print red(bright("*** ERROR - BUILD STOPPED"))
          return -1
      else:
        print magenta(bright("*** Skipping unsupported configuration: " + target + " " + system + " " + mode))
    print green(bright("*** BUILD COMPLETE"))
  else:
    print_usage()
  return 0

if __name__ == "__main__":
  dir = os.path.dirname(os.path.realpath(sys.argv[0]))
  with cd(dir):
    exit(main(sys.argv[1:]))

