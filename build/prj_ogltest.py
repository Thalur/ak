#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
import os
import shutil
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
    self.client = "-D_AK_CLIENT=0"

  def get_supported_systems(self):
    return [ "win", "linux", "osx", "android", "ios" ]

  def pre_build_actions(self):
    if self.system == "android":
      # Build Java stuff and the native android resources to R.java
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
      pass
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
    print magenta(bright("TODO: OSX post-actions"))
    return False

  def post_build_actions_android(self, binDir):
    android_platform = os.environ['ANDROID_PLATFORM']
    print "Detected Android Platform as " + android_platform
    apkDir = "apk"
    if os.path.exists(apkDir):
      shutil.rmtree(apkDir)
    os.makedirs(apkDir+"/lib/armeabi") # Shared library directory
    os.makedirs(apkDir+"/res/drawable") # Android resources (icons)
    os.makedirs(apkDir+"/assets") # Binary asset files (.ak)
    with cd(apkDir):

      # Copy everything we need into the apk directory
      #shutil.copy2("../classes.dex", "./")
      build.copy_files("../projects/ogltest/*.so", "lib/armeabi/")
      shutil.copy2("../../../../../src/projects/ogltest/android/AndroidManifest.xml", "./")
      copy_files("../../../../../src/projects/ogltest/android/res/drawable/*", "res/drawable/")
      copy_files("../../../../../src/projects/ogltest/*.ak", "assets/")

      # Create the APK
      buildCmd = "aapt package -f -M ./AndroidManifest.xml -S res/ -A assets/ -I " + android_platform + "/android.jar -F OGLtest.apk.unaligned"
      if call(buildCmd, shell=True) != 0:
        return False
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
