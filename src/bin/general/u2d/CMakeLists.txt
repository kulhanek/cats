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
QT5_USE_MODULES(u2d Core)
TARGET_LINK_LIBRARIES(u2d ${HIPOLY_LIB_NAME} ${SYSTEM_LIBS})

INSTALL(TARGETS
            u2d
        DESTINATION
            bin
        )
