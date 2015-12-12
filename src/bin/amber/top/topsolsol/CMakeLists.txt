# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPSOLSOL_SRC
        main.cpp
        TopSolSol.cpp
        TopSolSolOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topsolsol ${TOPSOLSOL_SRC})
ADD_DEPENDENCIES(topsolsol cats_shared)

QT5_USE_MODULES(topsolsol Core)
TARGET_LINK_LIBRARIES(topsolsol
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topsolsol
        DESTINATION
            bin
        )
