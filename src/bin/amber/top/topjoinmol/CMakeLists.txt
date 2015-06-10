# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPJOINMOL_SRC
        main.cpp
        TopJoinMol.cpp
        TopJoinMolOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topjoinmol ${TOPJOINMOL_SRC})
QT5_USE_MODULES(topjoinmol Core)
TARGET_LINK_LIBRARIES(topjoinmol
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topjoinmol
        DESTINATION
            bin
        )
