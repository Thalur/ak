project (Client.Gfx)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

SET (CLIENT_GFX_FILES
    Engine.cpp
    Engine.h
    GraphicsComponent.cpp
    GraphicsComponent.h
    IEngine.h
    oglincludes.h
    pngLoader.cpp
    pngLoader.h
    Texture.h)

add_library(Client.Gfx ${CLIENT_GFX_FILES})
