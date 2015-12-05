#!/usr/bin/env python
#
# Multi-platform and multi-target build configuration script for cmake
# Copyright (C) 2015 by Andre Koschmieder
#
import shutil

import build
from colored import *


class project_akcab:
  'Build definitions for the project akcab'

  def __init__(self, target, system, mode, install):
    self.target = target
    self.system = system
    self.mode = mode
    self.install = install
    self.config = "-D_AK_PROJECT=AKCAB"
    self.client = "-D_AK_CLIENT=0"

  def get_supported_systems(self):
    return [ "win", "linux", "osx" ]

  def pre_build_actions(self):
    return True

  def post_build_actions_win(self, binDir):
    build.copy_files("projects/akcab/" + self.mode + "/*.exe", binDir)
    build.copy_files("projects/akcab/" + self.mode + "/*.pdb", binDir)
    return True

  def post_build_actions_linux(self, binDir):
    shutil.copy2("projects/akcab/AKCab", binDir)
    return True

  def post_build_actions_osx(self, binDir):
    shutil.copy2("projects/akcab/AKCab", binDir)
    return True
