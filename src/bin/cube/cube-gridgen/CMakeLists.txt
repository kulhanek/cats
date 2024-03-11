# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(CUBEGRIDGEN_SRC
        main.cpp
        CubeGridGen.cpp
        CubeGridGenOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(cube-gridgen ${CUBEGRIDGEN_SRC})
ADD_DEPENDENCIES(cube-gridgen cats_shared)

TARGET_LINK_LIBRARIES(cube-gridgen Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            cube-gridgen
        DESTINATION
            bin
        )

