
IF ("${AK_PROJECT}" STREQUAL "AKCAB")
  add_subdirectory(akcab)
ELSEIF ("${AK_PROJECT}" STREQUAL "ANIMVIEW")
  add_subdirectory(animview)
ELSE ()
  message(FATAL_ERROR "*** Unrecognized target")
ENDIF ()
