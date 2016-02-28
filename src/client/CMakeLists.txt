project (Client.Common)

SET (CLIENT_FILES
    ResourceManager.cpp
    ResourceManager.h)

add_library(Client.Common ${CLIENT_FILES})

add_subdirectory (main)
add_subdirectory (gfx)
