#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
import glob
import os
import shutil
from subprocess import call

import build
from colored import *


class project_ogltest:
  'Build definitions for the project akcab'

  def __init__(self, target, system, mode, install):
    self.target = target
    self.system = system
    self.mode = mode
    self.install = install
    self.config = "-D_AK_PROJECT=OGLTEST"
    self.client = "-D_AK_CLIENT=1"

  def get_supported_systems(self):
    return [ "win", "linux", "osx", "android", "ios" ]

  def pre_build_actions(self):
    # Android only needs the app resources for compilation, the assets can be created later
    if self.system in ["android", "ios"]:
      #android_platform = os.environ['ANDROID_PLATFORM']
      #print "Detected Android Platform as " + android_platform
      ## Build Java stuff and the native android resources to R.java
      #if os.path.exists("class"):
      #  shutil.rmtree("class")
      #os.makedirs("class")
      #print green("Building the Java project...");
      #buildCmd = "javac -cp "+android_platform+"/android.jar -d class ../../../../src/projects/ogltest/android/LoadLibraries.java"
      #if call(buildCmd, shell=True) != 0:
      #  return False
      #buildCmd = "dx --dex --output=classes.dex class/"
      #if call(buildCmd, shell=True) != 0:
      #  return False
      # ToDo
      # aapt package -m -J gen/ -M ./AndroidManifest.xml -S res1/ -S res2 ... -I android.jar
      return True
    else:
      return self.create_assets(".")

  def create_assets(self, binDir):
    # create one .ak file for each subdirectory in projects/ogltest/data
    with build.cd("../../../../src/projects/ogltest/data"):
      for file in glob.glob("*.ak"):
        os.remove(file)
      for folder in os.listdir("."):
        if os.path.isdir(folder):
          print "Creating asset file " + folder + ".ak from folder " + folder + " ..."
          if self.system == "android":
            sys = "linux"
          elif self.system == "ios":
            sys = "osx"
          else:
            sys = self.system
          exe = os.path.join("..", "..", "..", "..", "gen", "akcab", sys, "debug", "AKCab")
          cmd = exe + " " + folder + ".ak -add -recursive " + folder
          if call(cmd, shell=True) != 0:
            return False
    build.copy_files("../../../../src/projects/ogltest/data/*.ak", binDir)
    return True

  def post_build_actions_win(self, binDir):
    build.copy_files("projects/ogltest/" + self.mode + "/*.exe", binDir)
    build.copy_files("projects/ogltest/" + self.mode + "/*.pdb", binDir)
    shutil.copy2("../../../../lib/freeglut/bin/x64/freeglut.dll", binDir)
    shutil.copy2("../../../../lib/glew-1.12.0/bin/Release/x64/glew32.dll", binDir)
    return True

  def post_build_actions_linux(self, binDir):
    shutil.copy2("projects/ogltest/OGLtest", binDir)
    return True

  def post_build_actions_osx(self, binDir):
    shutil.copy2("projects/ogltest/OGLtest", binDir)
    build.copy_files("*.ak", binDir)
    return True

  def post_build_actions_android(self, binDir):
    android_platform = os.environ['ANDROID_PLATFORM']
    print "Detected Android Platform as " + android_platform
    apkDir = "apk"
    if os.path.exists(apkDir):
      shutil.rmtree(apkDir)
    os.makedirs(apkDir+"/lib/armeabi") # Shared library directory
    os.makedirs(apkDir+"/assets") # Binary asset files (.ak)
    if not self.create_assets(apkDir+"/assets"):
      return False
    with build.cd(apkDir):

      # Copy everything we need into the apk directory
      #shutil.copy2("../classes.dex", "./")
      build.copy_files("../projects/ogltest/*.so", "lib/armeabi/")
      #shutil.copy2("../projects/ogltest/libpng/libpng16d.so", "lib/armeabi/libpng16.so")
      shutil.copy2("../../../../../src/projects/ogltest/AndroidManifest.xml", "./")
      shutil.copytree("../../../../../src/projects/ogltest/res", "res")

      # Create the APK
      buildCmd = "aapt package -f -M ./AndroidManifest.xml -S res/ -A assets/ -I " + android_platform + "/android.jar -F OGLtest.apk.unaligned"
      if call(buildCmd, shell=True) != 0:
        return False
      #buildCmd = "aapt add -f OGLtest.apk.unaligned classes.dex lib/armeabi/*"
      buildCmd = "aapt add -f OGLtest.apk.unaligned lib/armeabi/*"
      if call(buildCmd, shell=True) != 0:
        return False

      # Sign the package
      buildCmd = "jarsigner -sigalg MD5withRSA -digestalg SHA1 -storepass test1234 -keypass test1234 -keystore ~/akkeystore OGLtest.apk.unaligned ogltestkey"
      if call(buildCmd, shell=True) != 0:
        return False

      # Run zipalign
      buildCmd = "zipalign 4 OGLtest.apk.unaligned OGLtest.apk"
      if call(buildCmd, shell=True) != 0:
        return False

    shutil.copy2("apk/OGLtest.apk", binDir)
    if self.install:
      call("adb install -rdg " + binDir + "/OGLtest.apk", shell=True)
    return True
