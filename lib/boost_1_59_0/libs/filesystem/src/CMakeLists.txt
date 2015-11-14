project (Boost.Filesystem)

add_library (Boost.Filesystem codecvt_error_category.cpp operations.cpp path.cpp path_traits.cpp portability.cpp unique_path.cpp utf8_codecvt_facet.cpp windows_file_codecvt.cpp)
target_link_libraries (Boost.Filesystem Boost.System)
