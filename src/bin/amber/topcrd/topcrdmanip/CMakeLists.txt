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
QT5_USE_MODULES(topcrdmanip Core)
ADD_DEPENDENCIES(topcrdmanip cats_shared)

TARGET_LINK_LIBRARIES(topcrdmanip
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrdmanip
        DESTINATION
            bin
        )

