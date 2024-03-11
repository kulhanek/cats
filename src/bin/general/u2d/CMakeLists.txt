# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(U2D_SRC
        u2d.cpp
        u2d-opts.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(u2d ${U2D_SRC})
ADD_DEPENDENCIES(u2d cats_shared)

TARGET_LINK_LIBRARIES(u2d Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            u2d
        DESTINATION
            bin
        )
