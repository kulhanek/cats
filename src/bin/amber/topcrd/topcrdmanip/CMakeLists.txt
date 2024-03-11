# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRDMANIP_SRC
        main.cpp
        TopCrdManip.cpp
        TopCrdManipOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrdmanip ${TOPCRDMANIP_SRC})
ADD_DEPENDENCIES(topcrdmanip cats_shared)

TARGET_LINK_LIBRARIES(topcrdmanip Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrdmanip
        DESTINATION
            bin
        )

