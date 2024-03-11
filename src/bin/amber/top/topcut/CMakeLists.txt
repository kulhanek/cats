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

TARGET_LINK_LIBRARIES(topcut Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcut
        DESTINATION
            bin
        )

