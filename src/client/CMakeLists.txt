project (Client)

SET (CLIENT_FILES
    ResourceManager.cpp
    ResourceManager.h)

add_library(Client ${CLIENT_FILES})
target_link_libraries (Client Client.Main Client.Gfx)

add_subdirectory (main)
add_subdirectory (gfx)
