# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2TEMP_SRC
        main.cpp
        TopCrd2Temp.cpp
        TopCrd2TempOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2temp ${TOPCRD2TEMP_SRC})
QT5_USE_MODULES(topcrd2temp Core)
TARGET_LINK_LIBRARIES(topcrd2temp
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2temp
        DESTINATION
            bin
        )
