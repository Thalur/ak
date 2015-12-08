project (akpng)

include_directories (${ZLIB_INCLUDE_DIR})

set (akpng_src_files
  png.c pngerror.c pngget.c
  pngmem.c pngpread.c pngread.c
  pngrio.c pngrtran.c pngrutil.c
  pngset.c pngtrans.c pngwio.c
  pngwrite.c pngwtran.c pngwutil.c
)

add_library (akpng STATIC ${akpng_src_files})
SET_TARGET_PROPERTIES (akpng PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (akpng ${ZLIB_LIBRARY})

#IF ("${AK_TARGET}" STREQUAL "ANDROID")
#  set_target_properties (akpng PROPERTIES COMPILE_DEFINITIONS "ANDROID")
#ENDIF ()
