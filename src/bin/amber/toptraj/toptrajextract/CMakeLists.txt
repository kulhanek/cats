# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2CRD_SRC
        main.cpp
        TopTrajExtract.cpp
        TopTrajExtractOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(toptrajextract ${TOPCRD2CRD_SRC})
QT5_USE_MODULES(toptrajextract Core)
TARGET_LINK_LIBRARIES(toptrajextract
        ${CATS_LIBS})

INSTALL(TARGETS
            toptrajextract
        DESTINATION
            bin
        )
