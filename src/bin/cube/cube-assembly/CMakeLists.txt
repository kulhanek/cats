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

QT5_USE_MODULES(cube-assembly Core)
TARGET_LINK_LIBRARIES(cube-assembly
        ${CATS_LIBS})

INSTALL(TARGETS
            cube-assembly
        DESTINATION
            bin
        )

