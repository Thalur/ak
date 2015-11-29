#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
# ToDo list:
#  * add missing build target systems: android, osx, ios
#  * generalize build configurations by having one config file per target
#
import sys
import os
import glob
import shutil
from subprocess import call

from colored import *

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
glob_target_requires_client = [
  #akcab, ogltest, animview
   False, False,   True ]


def print_usage():
  print cyan(bright("USAGE: python build.py [<TARGET>+] [<SYSTEM>] [<MODE>] [CLEAN]"))
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
  config = "-D_AK_PROJECT=" + target.upper()
  client = "-D_AK_CLIENT=0"
  target_index = option_contains(glob_targets, target)
  if glob_target_requires_client[target_index]:
    client = "-D_AK_CLIENT=1"
  cmakeCmd = [ 'cmake', '-G', 'Visual Studio 12 2013 Win64', config, client, '-D_AK_TARGET=WIN64', '../../../../src']
  if call(cmakeCmd, shell=True) != 0:
    return False
  buildCmd = "cmake --build . --config " + mode.upper()
  return call(buildCmd, shell=True) == 0

def build_linux(target, mode):
  config = "-D_AK_PROJECT=" + target.upper()
  client = "-D_AK_CLIENT=0"
  target_index = option_contains(glob_targets, target)
  if glob_target_requires_client[target_index]:
    client = "-D_AK_CLIENT=1"
  cmakeCmd = [ 'cmake', '-G', 'Unix Makefiles', config, client, '-D_AK_TARGET=LINUX', "-DCMAKE_BUILD_TYPE="+mode, '../../../../src']
  if call(cmakeCmd) != 0:
    return False
  return call('make') == 0

def build_android(target, mode):
  # http://spin.atomicobject.com/2011/08/22/building-android-application-bundles-apks-by-hand/
  config = "-D_AK_PROJECT=" + target.upper()
  client = "-D_AK_CLIENT=0"
  target_index = option_contains(glob_targets, target)
  if glob_target_requires_client[target_index]:
    client = "-D_AK_CLIENT=1"
  ndk = os.environ['ANDROID_NDK']
  print "Detected NDK path as " + ndk
  android_platform = os.environ['ANDROID_PLATFORM']
  print "Detected Android Platform as " + android_platform

  # 1. Generate the cmake-android toolchain
  cmakeCmd = [ 'cmake', '-DCMAKE_TOOLCHAIN_FILE=../../../../lib/android-cmake/android.toolchain.cmake',
               '-DANDROID_ABI=armeabi', '-DANDROID_NATIVE_API_LEVEL=android-9',
               '-DANDROID_NDK='+ndk, '-DCMAKE_VERBOSE_MAKEFILE=OFF',
               config, client, '-D_AK_TARGET=ANDROID', "-DCMAKE_BUILD_TYPE="+mode, '../../../../src']
  if call(cmakeCmd) != 0:
    return False

  # 2. Build Java stuff and the native android resources to R.java
  #if os.path.exists("class"):
  #  shutil.rmtree("class")
  #os.makedirs("class")
  #buildCmd = "javac -cp "+android_platform+"/android.jar -d class ../../../../src/projects/ogltest/android/LoadLibraries.java"
  #if call(buildCmd, shell=True) != 0:
  #  return False
  #buildCmd = "dx --dex --output=classes.dex class/"
  #if call(buildCmd, shell=True) != 0:
  #  return False
  # ToDo
  # aapt package -m -J gen/ -M ./AndroidManifest.xml -S res1/ -S res2 ... -I android.jar

  # 3. Run the build with cmake
  buildCmd = "cmake --build . --config " + mode.upper()
  if call(buildCmd, shell=True) != 0:
    return False

  # 4. Copy everything we need into the apk directory
  apkDir = "apk"
  if os.path.exists(apkDir):
    shutil.rmtree(apkDir)
  os.makedirs(apkDir+"/lib/armeabi") # Shared library directory
  os.makedirs(apkDir+"/res/drawable") # Android resources (icons)
  os.makedirs(apkDir+"/assets") # Binary asset files (.ak)
  with cd(apkDir):
    
    #shutil.copy2("../classes.dex", "./")
    copy_files("../projects/"+target+"/*.so", "lib/armeabi/")
    shutil.copy2("../../../../../src/projects/"+target+"/android/AndroidManifest.xml", "./")
    copy_files("../../../../../src/projects/"+target+"/android/res/drawable/*", "res/drawable/")
    copy_files("../../../../../src/projects/"+target+"/*.ak", "assets/")

    # 5. Create the APK
    buildCmd = "aapt package -f -M ./AndroidManifest.xml -S res/ -A assets/ -I "+android_platform+"/android.jar -F OGLtest.apk.unaligned"
    if call(buildCmd, shell=True) != 0:
      return False
    buildCmd = "aapt add -f OGLtest.apk.unaligned lib/armeabi/*"
    if call(buildCmd, shell=True) != 0:
      return False

    # 6. Sign the package
    buildCmd = "jarsigner -sigalg MD5withRSA -digestalg SHA1 -storepass test1234 -keypass test1234 -keystore ~/akkeystore OGLtest.apk.unaligned ogltestkey"
    if call(buildCmd, shell=True) != 0:
      return False

    # 7. Run zipalign
    buildCmd = "zipalign 4 OGLtest.apk.unaligned OGLtest.apk"
    if call(buildCmd, shell=True) != 0:
      return False

  return True

#####

def build_nyi(target, mode):
  print magenta(bright("This configuration has not been implemented yet."))
  return False

def copy_files(source, target):
  for file in glob.glob(source):
    shutil.copy2(file, target)

def run_build(target, system, mode, clean):
  try:
    print cyan(bright("\n*** Starting build of " + target + " for " + system + " (" + mode + ")..."))
    buildResult = False;
    outputDir = os.path.join("..", "gen", "cmake", target, system)
    if clean and os.path.exists(outputDir):
      print "*** Cleaning workspace by removing " + outputDir
      shutil.rmtree(outputDir)
    if not os.path.exists(outputDir):
      os.makedirs(outputDir)
    with cd(outputDir):
      build_fct = "build_" + system
      buildResult = globals().get(build_fct, build_nyi)(target, mode)
    if buildResult:
      binDir = os.path.join("..", "gen", target, system, mode)
      if not os.path.exists(binDir):
        os.makedirs(binDir)
      # TODO: generalize for all projects / targets
      if system == "win":
        copy_files(outputDir+"/projects/"+target+"/"+mode+"/*.exe", binDir)
        copy_files(outputDir+"/projects/"+target+"/"+mode+"/*.pdb", binDir)
        if target == "ogltest" or target == "animview":
          shutil.copy2("../lib/freeglut/bin/x64/freeglut.dll", binDir)
          shutil.copy2("../lib/glew-1.12.0/bin/Release/x64/glew32.dll", binDir)
      elif system == "linux":
        if target == "akcab":
          shutil.copy2(outputDir+"/projects/"+target+"/AKCab", binDir)
        elif target == "ogltest":
          shutil.copy2(outputDir+"/projects/"+target+"/OGLtest", binDir)
        elif target == "animview":
          shutil.copy2(outputDir+"/projects/"+target+"/AnimView", binDir)
      elif system == "android":
        shutil.copy2(outputDir+"/apk/OGLtest.apk", binDir)
        call("adb install -rdg ../gen/ogltest/android/debug/OGLtest.apk", shell=True)
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
  system = get_system(argv, host_os)
  mode = get_mode(argv)
  clean = get_clean(argv)
  targets = get_targets(argv, system)
  if system and targets and mode:
    for target in targets:
      if check_config(target, system, mode):
        if not run_build(target, system, mode, clean):
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

