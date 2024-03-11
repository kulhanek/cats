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
ADD_DEPENDENCIES(topjoinmol cats_shared)

TARGET_LINK_LIBRARIES(topjoinmol Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topjoinmol
        DESTINATION
            bin
        )
