# ==============================================================================
# CATs - Conversion and Analysis Tools
# ==============================================================================

# program objects --------------------------------------------------------------
SET(COMPACTRES_SRC
        main.cpp
        CompactRes.cpp
        CompactResOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(compactres ${COMPACTRES_SRC})
ADD_DEPENDENCIES(compactres cats_shared)

TARGET_LINK_LIBRARIES(compactres Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            compactres
        DESTINATION
            bin
        )
