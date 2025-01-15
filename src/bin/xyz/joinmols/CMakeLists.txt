# ==============================================================================
# CATs - Conversion and Analysis Tools
# ==============================================================================

# program objects --------------------------------------------------------------
SET(JOINMOLS_SRC
        main.cpp
        JoinMols.cpp
        JoinMolsOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(joinmols ${JOINMOLS_SRC})
ADD_DEPENDENCIES(joinmols cats_shared)

TARGET_LINK_LIBRARIES(joinmols Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            joinmols
        DESTINATION
            bin
        )
