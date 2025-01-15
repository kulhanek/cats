# ==============================================================================
# CATs - Conversion and Analysis Tools
# ==============================================================================

# program objects --------------------------------------------------------------
SET(MATCHATOMS_SRC
        main.cpp
        MatchAtoms.cpp
        MatchAtomsOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(matchatoms ${MATCHATOMS_SRC})
ADD_DEPENDENCIES(matchatoms cats_shared)

TARGET_LINK_LIBRARIES(matchatoms Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            matchatoms
        DESTINATION
            bin
        )
