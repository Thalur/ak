#!/usr/bin/env python
#
# Convenience functions for colored text output with colorama
# Copyright (C) 2015 by Andre Koschmieder
#
from colorama import Fore, Back, Style

def init_color(os):
  if os == "win":
    import colorama
    colorama.init()

def color(color, brightness, text):
  return color + brightness + text + Style.RESET_ALL

def red(text):
  return Fore.RED + text + Fore.RESET

def green(text):
  return Fore.GREEN + text + Fore.RESET

def yellow(text):
  return Fore.YELLOW + text + Fore.RESET

def blue(text):
  return Fore.BLUE + text + Fore.RESET

def magenta(text):
  return Fore.MAGENTA + text + Fore.RESET

def cyan(text):
  return Fore.CYAN + text + Fore.RESET

def white(text):
  return Fore.WHITE + text + Fore.RESET

def bright(text):
  return Style.BRIGHT + text + Style.RESET_ALL

def dim(text):
  return Style.DIM + text + Style.RESET_ALL
