# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCUT_SRC
        main.cpp
        TopCut.cpp
        TopCutOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcut ${TOPCUT_SRC})
ADD_DEPENDENCIES(topcut cats_shared)

QT5_USE_MODULES(topcut Core)
TARGET_LINK_LIBRARIES(topcut
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topcut
        DESTINATION
            bin
        )

