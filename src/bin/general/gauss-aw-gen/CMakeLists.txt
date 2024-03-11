# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(INTEGRATE_SRC
        gauss-aw-gen.cpp
        gauss-aw-gen-opts.cpp
        main.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(gauss-aw-gen ${INTEGRATE_SRC})
ADD_DEPENDENCIES(gauss-aw-gen cats_shared)

TARGET_LINK_LIBRARIES(gauss-aw-gen Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            gauss-aw-gen
        DESTINATION
            bin
        )
