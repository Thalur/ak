project (Client.Gfx)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

add_library(Client.Gfx Engine.cpp pngLoader.cpp)
