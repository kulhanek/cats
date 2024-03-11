# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(CUBEASSEMBLY_SRC
        main.cpp
        CubeAssembly.cpp
        CubeAssemblyOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(cube-assembly ${CUBEASSEMBLY_SRC})
ADD_DEPENDENCIES(cube-assembly cats_shared)

TARGET_LINK_LIBRARIES(cube-assembly Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            cube-assembly
        DESTINATION
            bin
        )

